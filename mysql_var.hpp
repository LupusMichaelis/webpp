#ifndef HPP_MYSQL_VAR
#	define HPP_MYSQL_VAR

#	include "memory.hpp"

namespace webpp { namespace mysql {

class var
{
	class impl;
	std::unique_ptr<impl> mp_impl;

	public:

		var();
		var(var const & copied);
		~var();

		operator bool() const;

		var & operator=(var const & copied);

		var & operator=(std::nullptr_t const & null);
		bool operator ==(std::nullptr_t) const;

		var & operator=(std::string const & value);
		std::string const & string() const;
		bool const is_string() const;

		var & operator=(int const & value);
		int const integer() const;
		bool const is_integer() const;

		var & operator=(bool const & value);
		bool const boolean() const;
		bool const is_boolean() const;

};

} } // namespace webpp::mysql

inline bool operator ==(std::nullptr_t, webpp::mysql::var const & var)
{
	return var == nullptr;
}

#endif // HPP_MYSQL_VAR
