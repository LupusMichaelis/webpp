
#include "configuration.hpp"

#include <algorithm>

static std::map<std::string, std::string> const make_environment();
extern char const ** environ;

namespace webpp {
configuration::configuration(int argc, char const * argv[])
	: m_args {argv, argv + argc}
	, m_environment {make_environment()}
{
}
} // namespace webpp

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

