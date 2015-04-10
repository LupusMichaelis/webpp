
#include "http_response.hpp"

namespace webpp { namespace http {

response::response()
	: m_status {200}
{
}

response::~response()
{
}

std::string const & response::body() const
{
	return m_body;
}

int const response::status() const
{
	return m_status;
}

void response::status(int const new_status)
{
	m_status = new_status;
}

size_t const response::size() const
{
	return m_body.size();
}

}} // namespace webpp::http
