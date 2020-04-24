
#include "http_request.hpp"
#include "configuration.hpp"

#include <map>
#include <string>
#include <algorithm>
#include <iostream>

namespace webpp { namespace http {

bool from_cgi(configuration const & c, std::unique_ptr<request> & p_r_out)
{
	auto const & env = c.get_environment();

	auto p_r = std::make_unique<request>();
	try
	{
		p_r->method(env.at("REQUEST_METHOD"));
		p_r->uri(env.at("REQUEST_URI"));
		p_r->content_type(env.at("CONTENT_TYPE"));

		std::swap(p_r, p_r_out);
	}
	catch(std::out_of_range const & )
	{
		return false;
	}

	return true;
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
