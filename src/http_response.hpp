#ifndef HPP_RESPONSE_HTTP_WEBPP
#	define HPP_RESPONSE_HTTP_WEBPP

#	include <string>

namespace webpp { namespace http {

class response
{
	public:
		response();
		~response();

		std::string const & body() const;

		int const status() const;
		void status(int const new_status);

		size_t const size() const;

	private:

		int m_status;
		std::string m_body;
};

}} // namespace webpp::http

#endif // HPP_RESPONSE_HTTP_WEBPP

