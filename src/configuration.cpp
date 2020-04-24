
#include "configuration.hpp"

#include <algorithm>

extern char const ** environ;

namespace webpp {

static std::map<std::string, std::string> const make_environment();

configuration::configuration(int argc, char const * argv[])
	: m_args {argv, argv + argc}
	, m_environment {make_environment()}
{
}

configuration::configuration(int argc, char * argv[])
	: m_args {argv, argv + argc}
	, m_environment {make_environment()}
{
}

static std::map<std::string, std::string> const make_environment()
{
	std::map<std::string, std::string> environment;

	char const ** p = environ;
	while(*p)
	{
		std::string const pair {*p};
		auto colon = std::find(pair.cbegin(), pair.cend(), '=');
		if(pair.cend() != colon)
		{
			std::string key {pair.cbegin(), colon};
			std::string value {colon + 1, pair.cend()};

			environment[key] = value;
		}

		++p;
	}

	return environment;
}

} // namespace webpp
