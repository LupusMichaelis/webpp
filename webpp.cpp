#include "src/convert_rows_to_json.hpp"
#include "src/router.hpp"
#include "src/http_request.hpp"
#include "src/http_response.hpp"
#include "src/json.hpp"
#include "src/json_parser.hpp"
#include "src/json_dump.hpp"
#include "src/model.hpp"
#include "src/mysql_connection.hpp"
#include "src/query_schema.hpp"
#include "src/query_var.hpp"

#include <algorithm>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <fstream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>

#include <boost/assign/std/map.hpp>
#include <boost/algorithm/string.hpp>

void json_extract
	( std::unique_ptr<webpp::model::criteria_list_type> & p_submitted
	, std::istream & in
	)
{
	auto p_out = std::make_unique<std::remove_reference<decltype(p_submitted)>::type::element_type>();

	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	for(auto & property: dynamic_cast<webpp::json::object &>(*p_tree).properties())
	{
		// TODO visitor to convert json to mysql
		auto & value = dynamic_cast<webpp::json::string &>(*property.second).get();
		auto p_mysql_value = std::make_shared<webpp::query::string>(value);
		webpp::query::schema::field field{property.first};

		p_out->insert(std::make_pair(field, p_mysql_value));
	}

	std::swap(p_out, p_submitted);
}

/*
		("Status", "200 OK")
	//	("Status", "404 Not Found")
		("Content-type", "application/json")
	//	("Content-type", "text/html")
*/

class webpp_response_body
	: public webpp::http::response::body
{
	public:
		virtual void feed(webpp::model::field_list_type const & fields, webpp::model::row_list_type const & rows) = 0;
		virtual void print(std::ostream & out) const = 0;
		virtual ~webpp_response_body() {};
};

class webpp_response_body_json
	: public webpp_response_body
{
	public:
		void feed(webpp::model::field_list_type const & fields, webpp::model::row_list_type const & rows)
		{
			webpp::json::build(mp_content);
			webpp::cast(*mp_content, fields, rows);
		}

		void print(std::ostream & out) const
		{
			if(!mp_content)
				return;

			if(0u == mp_content->values().size())
				webpp::json::dump(out, webpp::json::null());
			else if(1u == mp_content->values().size())
				webpp::json::dump(out, static_cast<webpp::json::value&>(*mp_content->values()[0]));
			else
				webpp::json::dump(out, static_cast<webpp::json::value&>(*mp_content));
		}

	private:
		std::unique_ptr<webpp::json::array> mp_content;
};

class webpp_response_body_html
	: public webpp_response_body
{
	public:
		void feed(webpp::model::field_list_type const & fields, webpp::model::row_list_type const & rows)
		{
			m_json_view.feed(fields, rows);
		}

		void print(std::ostream & out) const
		{
			out << "<!DOCTYPE html>\n"
				"<html>\n"
				"\t<head>\n\t\t<title>" << "Some data" << "</title>\n\t</head>\n"
				"\t<body>\n"
				"\t\t";

			out << "\t\t\t<pre>\n";
			m_json_view.print(out);
			out << "\t\t\t</pre>\n";

			out << "\n"
				"\t</body>\n"
				"</html>\n";
		}

	private:
		webpp_response_body_json m_json_view;
};


class program
{
	public:
		program()
			: mp_request{std::make_unique<webpp::http::request>()}
			, mp_response{std::make_unique<webpp::http::response>()}
		{
		}

		int operator() ()
		{
			do_find_route();
			do_process();
			do_print_out();
			return 0;
		}

		void do_find_route()
		{
			webpp::http::from_cgi(mp_request);

			{
				auto it_found = std::find(m_supported_content_types.cbegin(), m_supported_content_types.cend(), mp_request->content_type());
				if(m_supported_content_types.cend() == it_found)
					mp_request->content_type(m_default_content_type);
			}

			m_router.parse(mp_request->uri());

			if(m_router.is_root())
				return;

			m_router.get_segment(m_table_name, 1);

			if(m_router.has_segment(2))
			{
				std::string criteria;
				m_router.get_segment(criteria, 2);

				boost::smatch results;
				boost::regex const re { "^([^=]+)=([^=]+)$" };
				auto match_options = boost::match_perl;
				if(boost::regex_match(criteria, results, re, match_options))
				{
					webpp::query::schema::field key {std::string {results[1].first, results[1].second}};

					std::string value {results[2].first, results[2].second};
					std::shared_ptr<webpp::query::value> p_value { std::make_shared<webpp::query::string>(value) };

					auto criteria = std::make_pair(key, p_value);
					m_criterias.insert(criteria);
				}
			}
		}

		void do_write_log(const char * p_message)
		{
			std::cerr << boost::format("LOG: '%s'\n") % p_message;
		}

		void do_process()
		{
			if("POST" != mp_request->method() and "GET" != mp_request->method())
				throw boost::format("Unknown method '%s'") % mp_request->method();

			if(m_router.is_root())
			{
				if("POST" == mp_request->method())
					; // Site wide request
				else// if("GET" == mp_request->method())
					; // give help infos
				return;
			}

			webpp::model m;

			auto p_con = std::make_shared<webpp::mysql::connection>();
			try
			{
				std::ifstream in {"conf/database.ini"};
				std::unique_ptr<webpp::json::value> p_conf;
				webpp::json::parse(p_conf, in);

				std::string host;
				std::string user;
				std::string password;
				std::string space;
				unsigned port;

				for(auto const & property: dynamic_cast<webpp::json::object&>(*p_conf).properties())
					if("host" == property.first)
					{
						auto const & json_literal = dynamic_cast<webpp::json::string&>(*property.second);
						host = json_literal.get();
					}
					else if("user" == property.first)
					{
						auto const & json_literal = dynamic_cast<webpp::json::string&>(*property.second);
						user = json_literal.get();
					}
					else if("password" == property.first)
					{
						auto const & json_literal = dynamic_cast<webpp::json::string&>(*property.second);
						password = json_literal.get();
					}
					else if("space" == property.first)
					{
						auto const & json_literal = dynamic_cast<webpp::json::string&>(*property.second);
						space = json_literal.get();
					}
					else if("port" == property.first)
					{
						auto const & json_literal = dynamic_cast<webpp::json::number&>(*property.second);
						port = std::atoi(json_literal.get().c_str());
					}

				p_con->connect(host, user, password, space, port);
			}
			catch(const char* p_message)
			{
				mp_response->status(500);
				do_write_log(p_message);
				return;
			}

			m.connection(p_con);

			m.get_by_criterias(mp_fields, mp_rows, m_table_name, m_criterias);

			if("POST" == mp_request->method())
			{
				std::unique_ptr<webpp::model::criteria_list_type> p_submitted;
				json_extract(p_submitted, std::cin);

				webpp::model::row_list_type updated_rows;
				for(auto row: *mp_rows)
				{
					webpp::model::row_type updated_row;
					for(size_t idx = 0; idx < mp_fields->size(); ++idx)
					{
						webpp::query::schema::field field {mp_fields->at(idx)};
						auto const & p_value = (*p_submitted)[field];

						updated_row.push_back(p_value);
					}

					updated_rows.push_back(updated_row);
				}

				m.update(m_table_name, *mp_fields, *mp_rows, updated_rows, m_criterias);
			}
		}

		void do_print_out()
		{
			std::cout << "\n";// end of headers
			if("GET" == mp_request->method())
			{
				if(200 == mp_response->status())
					print(std::cout, *mp_fields, *mp_rows);
				else
				{
					std::unique_ptr<webpp_response_body> p_body;
					if("application/json" == mp_request->content_type())
						p_body = std::make_unique<webpp_response_body_json>();
					else if("text/html" == mp_request->content_type())
						p_body = std::make_unique<webpp_response_body_html>();
					else
						throw "No default view";
					p_body->print(std::cout);
				}
			}
		}

		void print(std::ostream & out, webpp::model::field_list_type const & fields, webpp::model::row_list_type const & rows)
		{
			std::unique_ptr<webpp_response_body> p_body;
			if("application/json" == mp_request->content_type())
				p_body = std::make_unique<webpp_response_body_json>();
			else if("text/html" == mp_request->content_type())
				p_body = std::make_unique<webpp_response_body_html>();
			else
				throw "No default view";

			p_body->feed(fields, rows);
			p_body->print(out);
		}

	private:
		std::unique_ptr<webpp::http::request>			mp_request;
		std::unique_ptr<webpp::http::response>			mp_response;

		webpp::router::actual m_router;

		std::string										m_table_name;
		webpp::model::criteria_list_type				m_criterias;
		std::unique_ptr<webpp::model::row_list_type>	mp_rows;
		std::unique_ptr<webpp::model::field_list_type>	mp_fields;

		static std::vector<std::string> const			m_supported_content_types;
		static std::string const						m_default_content_type;
};

std::vector<std::string> const program::m_supported_content_types {"application/json", "text/html"};
std::string const program::m_default_content_type = program::m_supported_content_types[0];

int main()
{
	program m;
	return m();
}
