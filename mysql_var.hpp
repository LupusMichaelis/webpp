#ifndef HPP_MYSQL_VAR
#	define HPP_MYSQL_VAR

#	include "memory.hpp"

class mysql_var
{
	class impl;
	std::unique_ptr<impl> mp_impl;

	public:

		mysql_var();
		mysql_var(mysql_var const & copied);
		~mysql_var();

		operator bool() const;

		mysql_var & operator=(mysql_var const & copied);

		mysql_var & operator=(std::nullptr_t const & null);
		bool operator ==(std::nullptr_t) const;

		mysql_var & operator=(std::string const & value);
		std::string const & string() const;
		bool const is_string() const;

		mysql_var & operator=(int const & value);
		int const integer() const;
		bool const is_integer() const;

		mysql_var & operator=(bool const & value);
		bool const boolean() const;
		bool const is_boolean() const;

};

inline bool operator ==(std::nullptr_t, mysql_var const & var)
{
	return var == nullptr;
}

#endif // HPP_MYSQL_VAR
