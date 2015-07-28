#include "src/convert_rows_to_json.hpp"
#include "src/http_request.hpp"
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
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>

#include <boost/assign/std/map.hpp>
#include <boost/algorithm/string.hpp>

std::map<std::string, std::string> const make_header()
{
	using namespace boost::assign;

	std::map<std::string, std::string> header;
	insert(header)
		("Status", "200 OK")
	//	("Status", "404 Not Found")
		("Content-type", "application/json")
	//	("Content-type", "text/html")
		;

	return header;
}

void print_json(std::ostream & out, webpp::model::field_list_type fields, webpp::model::row_list_type rows)
{
	std::unique_ptr<webpp::json::array> p_array;
	webpp::json::build(p_array);
	webpp::cast(*p_array, fields, rows);

	if(0u == p_array->values().size())
		webpp::json::dump(out, webpp::json::null());
	else if(1u == p_array->values().size())
		webpp::json::dump(out, static_cast<webpp::json::value&>(*p_array->values()[0]));
	else
		webpp::json::dump(out, static_cast<webpp::json::value&>(*p_array));
}

void print_html(std::ostream & out, webpp::model::field_list_type fields, webpp::model::row_list_type rows)
{
	out << "<!DOCTYPE html>\n"
		"<html>\n"
		"\t<head>\n\t\t<title>" << "Some data" << "</title>\n\t</head>\n"
		"\t<body>\n"
		"\t\t";

	out << "\t\t\t<pre>\n";
	print_json(out, fields, rows);
	out << "\t\t\t</pre>\n";

	out << "\n"
		"\t</body>\n"
		"</html>\n";
}

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

int main()
{
	auto header = make_header();

	for(auto p: header)
		std::cout << p.first << ":" << p.second << "\n";
	std::cout << "\n";

	std::unique_ptr<webpp::http::request> p_request;
	webpp::http::from_cgi(p_request);

	std::vector<std::string> segments;
	boost::split(segments, p_request->uri(), boost::is_any_of("/"));

	std::string table_name {segments[1]};

	std::string criteria;
	webpp::model::criteria_list_type criterias;

	if(segments.size() > 2)
	{
		criteria = segments[2];

		boost::smatch results;
		boost::regex const re { "^([^=]+)=([^=]+)$" };
		auto match_options = boost::match_perl;
		if(boost::regex_match(criteria, results, re, match_options))
		{
			webpp::query::schema::field key {std::string {results[1].first, results[1].second}};

			std::string value {results[2].first, results[2].second};
			std::shared_ptr<webpp::query::value> p_value { std::make_shared<webpp::query::string>(value) };

			auto criteria = std::make_pair(key, p_value);
			criterias.insert(criteria);
		}
	}

	webpp::model m;
	std::unique_ptr<webpp::model::row_list_type> p_rows;
	std::unique_ptr<webpp::model::field_list_type> p_fields;

	auto p_con = std::make_shared<webpp::mysql::connection>();
	p_con->connect("localhost", "test", "test", "test", 3308);
	m.connection(p_con);

	m.get_by_criterias(p_fields, p_rows, table_name, criterias);

	if("GET" == p_request->method())
		print_json(std::cout, *p_fields, *p_rows);
	else if("POST" == p_request->method())
	{
		std::unique_ptr<webpp::model::criteria_list_type> p_submitted;
		json_extract(p_submitted, std::cin);

		webpp::model::row_list_type updated_rows;
		for(auto row: *p_rows)
		{
			webpp::model::row_type updated_row;
			for(size_t idx = 0; idx < p_fields->size(); ++idx)
			{
				webpp::query::schema::field field {p_fields->at(idx)};
				auto const & p_value = (*p_submitted)[field];

				updated_row.push_back(p_value);
			}

			updated_rows.push_back(updated_row);
		}

		m.update(table_name, *p_fields, *p_rows, updated_rows, criterias);
	}
	else
	{
		throw boost::format("Unknown method '%s'") % p_request->method();
	}


	return 0;
}
