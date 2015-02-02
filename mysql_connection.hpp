#ifndef HPP_CONNECTION_MYSQL_WEBPP
#	define HPP_CONNECTION_MYSQL_WEBPP

#	include "memory.hpp"

namespace webpp { namespace mysql {

class result;

class connection
{
	class impl;
	std::unique_ptr<impl> mp_impl;

	public:

		connection();
		connection(connection const & copied) = delete;
		~connection();

		operator bool() const;

		connection & operator=(connection const & copied) = delete;

		void connect
			( std::string const & host
			, std::string username
			, std::string password
			, std::string space
			, unsigned port
			);

	void query(std::unique_ptr<result> & p_result, std::string const & query);
};

} } // namespace webpp::mysql

#endif // HPP_CONNECTION_MYSQL_WEBPP
