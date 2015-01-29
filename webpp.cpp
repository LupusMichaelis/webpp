#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>


extern char ** environ;

std::map<std::string, std::string> const make_header()
{
	std::map<std::string, std::string> header;
	header["Content-type"] = "text/plain";

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
#include "story.hpp"

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

	model m;
	std::unique_ptr<story> p_story;

	if("GET" == method)
	{
		m.get_story_by_id(p_story, segments[2]);
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

	std::string title, content;
	p_story->get_title(title);
	p_story->get_content(content);

	std::cout << "<!DOCTYPE html>\n"
		"<html>\n"
		"\t<head>\n\t\t<title>" << title << "</title>\n\t</head>\n"
		"\t<body>\n"
		"\t\t" << content << "\n"
		"\t</body>\n"
		"</html>\n";

	return 0;
}

