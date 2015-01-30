#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>


extern char ** environ;

std::map<std::string, std::string> const make_header()
{
	std::map<std::string, std::string> header;
	//header["Content-type"] = "text/html";
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

#include "model.hpp"

void print_all(std::ostream & out, std::vector<std::map<std::string, std::string>> rows)
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
			out << boost::format("\"%s\": \"%s\"\n") % field.first % field.second;
		}
		out << " }";
	}
	out << " ]";

	/*
	std::cout << "<!DOCTYPE html>\n"
		"<html>\n"
		"\t<head>\n\t\t<title>" << title << "</title>\n\t</head>\n"
		"\t<body>\n"
		"\t\t" << content << "\n"
		"\t</body>\n"
		"</html>\n";
	*/
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
	std::string criteria {segments[2]};

	std::map<std::string, std::string> criterias;

	boost::smatch results;
	boost::regex const re { "(\\w+)=([^=]+)" };
	auto match_options = boost::match_perl;
	if(boost::regex_match(criteria, results, re, match_options))
	{
		model::criteria_type::value_type criteria
			{ {results[1].first, results[1].second }
			, { results[2].first, results[2].second } };
		criterias.insert(criteria);
	}

	model m;
	std::vector<std::map<std::string, std::string>> rows;

	if("GET" == method)
	{
		m.get_rows_by_criterias(rows, table_name, criterias);
		print_all(std::cout, rows);
	}
	else if("POST" == method)
	{
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
