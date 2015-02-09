#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>

extern char ** environ;

std::map<std::string, std::string> const make_header()
{
	std::map<std::string, std::string> header;
	//header["Content-type"] = "text/html";
	header["Status"] = "200 OK";
	//header["Status"] = "404 Not Found";
	header["Content-type"] = "application/json";

	return header;
}

std::map<std::string, std::string> const make_environment()
{
	char ** p = environ;
	std::map<std::string, std::string> environment;

	while(*++p)
	{
		std::string const pair {*p};
		auto colon = std::find(pair.cbegin(), pair.cend(), '=');
		if(pair.cend() != colon)
		{
			std::string key {pair.cbegin(), colon};
			std::string value {colon + 1, pair.cend()};

			environment[key] = value;
		}
	}

	return environment;
}

#include "src/mysql_connection.hpp"
#include "src/model.hpp"

void print_json(std::ostream & out, webpp::model::row_list_type rows)
{
	out << "[ ";
	for(auto row: rows)
	{
		out << "{ ";

		bool is_first = true;
		for(auto field: row)
		{
			if(!is_first || (is_first = false))
				out << ", ";
			if(nullptr == field.second)
				out << boost::format("\"%s\": null\n") % field.first;
			else if(field.second.is_integer())
				out << boost::format("\"%s\": %s\n") % field.first % field.second.integer();
			else if(field.second.is_boolean())
				out << boost::format("\"%s\": %s\n")
					% field.first
					% (field.second.boolean() ? "true" : "false");
			else
				out << boost::format("\"%s\": \"%s\"\n") % field.first % field.second.string();
		}
		out << " }";
	}
	out << " ]" << std::endl;
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

#include "src/json.hpp"

void json_extract
	( std::unique_ptr<std::map<std::string, std::string>> & p_changes
	, std::istream & in
	)
{
	auto p_out = std::make_unique<std::remove_reference<decltype(p_changes)>::type::element_type>();

	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	for(auto property: dynamic_cast<webpp::json::object &>(*p_tree).properties())
	{
		auto value = dynamic_cast<webpp::json::string &>(*property.second).value();

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
