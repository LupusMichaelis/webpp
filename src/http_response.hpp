#ifndef HPP_RESPONSE_HTTP_WEBPP
#	define HPP_RESPONSE_HTTP_WEBPP

#	include <string>

namespace webpp { namespace http {

class response
{
	public:
		class body
		{
			public:
				virtual void print(std::ostream & out) const = 0;
				virtual ~body() {};
		};

	public:
		response();
		~response();

		std::string const & body() const;
		void body(std::string const & new_body);

		int const status() const;
		void status(int const new_status);

		size_t const size() const;

	private:

		int m_status;
		std::string m_body;
};

}} // namespace webpp::http

#endif // HPP_RESPONSE_HTTP_WEBPP
