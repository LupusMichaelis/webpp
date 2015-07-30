#ifndef HPP_REQUEST_HTTP_WEBPP
#	define HPP_REQUEST_HTTP_WEBPP

#	include <string>
#	include "memory.hpp"

namespace webpp { namespace http {

class request;

void from_cgi(std::unique_ptr<request> & p_r);

class request
{
	public:
		request();
		~request();

	public:
		void method(std::string const & r);
		void uri(std::string const & u);
		void content_type(std::string const & ct);

		std::string const & method() const;
		std::string const & uri() const;
		std::string const & content_type() const;

	private:

		std::string m_method;
		std::string m_uri;
		std::string m_content_type;
};

}} // namespace webpp::http

#endif // HPP_REQUEST_HTTP_WEBPP
