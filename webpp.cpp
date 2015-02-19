#include <algorithm>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>

extern char ** environ;

#include <boost/assign/std/map.hpp>

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

#include "src/mysql_connection.hpp"
#include "src/model.hpp"
#include "src/convert_rows_to_json.hpp"
#include "src/json.hpp"

void print_json(std::ostream & out, webpp::model::row_list_type rows)
{
	std::unique_ptr<webpp::json::array> p_array;
	webpp::json::build(p_array);
	*p_array << rows;

	if(0u == p_array->values().size())
		webpp::json::dump(out, webpp::json::null());
	else if(1u == p_array->values().size())
		webpp::json::dump(out, static_cast<webpp::json::value&>(*p_array->values()[0]));
	else
		webpp::json::dump(out, static_cast<webpp::json::value&>(*p_array));
}

void print_html(std::ostream & out, webpp::model::row_list_type rows)
{
	out << "<!DOCTYPE html>\n"
		"<html>\n"
		"\t<head>\n\t\t<title>" << "Some data" << "</title>\n\t</head>\n"
		"\t<body>\n"
		"\t\t";

	out << "\t\t\t<pre>\n";
	print_json(out, rows);
	out << "\t\t\t</pre>\n";

	out << "\n"
		"\t</body>\n"
		"</html>\n";
}

#include "src/json_parser.hpp"

void json_extract
	( std::unique_ptr<std::map<std::string, std::string>> & p_changes
	, std::istream & in
	)
{
	auto p_out = std::make_unique<std::remove_reference<decltype(p_changes)>::type::element_type>();

	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	for(auto & property: dynamic_cast<webpp::json::object &>(*p_tree).properties())
	{
		auto & value = dynamic_cast<webpp::json::string &>(*property.second).get();

		p_out->insert(std::make_pair(property.first, value));
	}

	std::swap(p_out, p_changes);
}

int main()
{
	auto header = make_header();

	for(auto p: header)
		std::cout << p.first << ":" << p.second << "\n";
	std::cout << "\n";

	auto environment = make_environment();
	auto &method = environment["REQUEST_METHOD"];
	auto &uri = environment["REQUEST_URI"];

	std::vector<std::string> segments;
	boost::split(segments, uri, boost::is_any_of("/"));

	std::string table_name {segments[1]};

	std::string criteria;
	std::map<std::string, std::string> criterias;

	if(segments.size() > 2)
	{
		criteria = segments[2];

		boost::smatch results;
		boost::regex const re { "^([^=]+)=([^=]+)$" };
		auto match_options = boost::match_perl;
		if(boost::regex_match(criteria, results, re, match_options))
		{
			webpp::model::criteria_type::value_type criteria
				{ {results[1].first, results[1].second }
				, { results[2].first, results[2].second } };
			criterias.insert(criteria);
		}
	}

	webpp::model m;
	std::unique_ptr<webpp::model::row_list_type> p_rows;

	auto p_con = std::make_shared<webpp::mysql::connection>();
	p_con->connect("localhost", "test", "test", "test", 3308);
	m.connection(p_con);

	if("GET" == method)
	{
		m.get_rows_by_criterias(p_rows, table_name, criterias);
		print_json(std::cout, *p_rows);
	}
	else if("POST" == method)
	{
		std::unique_ptr<std::map<std::string, std::string>> p_changes;
		json_extract(p_changes, std::cin);

		m.get_rows_by_criterias(p_rows, table_name, criterias);
		for(auto row: *p_rows)
		{

		}

		m.update_rows(table_name, *p_changes, criterias);
	}
	else if("PUT" == method)
	{
	}
	else
	{
		throw boost::format("Unknown method '%s'") % method;
	}


	return 0;
}
