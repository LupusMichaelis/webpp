
#include "http_request.hpp"

#include <map>
#include <string>
#include <algorithm>

extern char const ** environ;

namespace webpp { namespace http {

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

void from_cgi(request & r)
{
	auto env = make_environment();
	r.method(env["REQUEST_METHOD"]);
	r.uri(env["REQUEST_URI"]);

}

request::~request()
{
}

request::request()
	: m_method(), m_uri()
{
}

void request::method(std::string const & m)
{
	m_method = m;
}

void request::uri(std::string const & u)
{
	m_uri = u;
}

std::string const & request::method() const
{
	return m_method;
}

std::string const & request::uri() const
{
	return m_uri;
}

}}; // namespace webpp::http
