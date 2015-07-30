
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

void from_cgi(std::unique_ptr<request> & p_r)
{
	p_r = std::make_unique<request>();
	auto env = make_environment();
	p_r->method(env["REQUEST_METHOD"]);
	p_r->uri(env["REQUEST_URI"]);
	p_r->content_type(env["CONTENT_TYPE"]);

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

std::string const & request::method() const
{
	return m_method;
}

void request::uri(std::string const & u)
{
	m_uri = u;
}

std::string const & request::uri() const
{
	return m_uri;
}

void request::content_type(std::string const & ct)
{
	m_content_type = ct;
}

std::string const & request::content_type() const
{
	return m_content_type;
}

}}; // namespace webpp::http
