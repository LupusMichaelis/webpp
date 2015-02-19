#ifndef HPP_REQUEST_HTTP_WEBPP
#	define HPP_REQUEST_HTTP_WEBPP

#	include <string>

namespace webpp { namespace http {

class request;

void from_cgi(request & r);

class request
{
	public:
		virtual ~request();
		request();

		void method(std::string const & r);
		void uri(std::string const & u);

		std::string const & method() const;
		std::string const & uri() const;

	private:

		std::string m_method;
		std::string m_uri;
};

}} // namespace webpp::http

#endif // HPP_REQUEST_HTTP_WEBPP
