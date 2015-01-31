
#include "mysql_var.hpp"

#include <string>

struct mysql_var::impl
{
	impl() : is_null(true), is_string(false), is_boolean(false), is_integer(false) {}

	bool		is_null;
	bool		is_string;
	bool		is_boolean;
	bool		is_integer;

	std::string	string;
	long long   integer;
	bool		boolean;
};

mysql_var::mysql_var()
	: mp_impl {std::make_unique<impl>()}
{
}

mysql_var::mysql_var(mysql_var const & copied)
	: mp_impl{std::make_unique<impl>(*copied.mp_impl)}
{
}

mysql_var::~mysql_var()
{
}

mysql_var & mysql_var::operator=(mysql_var const & copied)
{
	auto p_impl = std::make_unique<impl>(*copied.mp_impl);
	std::swap(p_impl, mp_impl);
	return *this;
}

mysql_var & mysql_var::operator=(std::nullptr_t const & null)
{
	*mp_impl = impl();
	return *this;
}

mysql_var & mysql_var::operator=(std::string const & value)
{
	*mp_impl = impl();
	mp_impl->string = value;
	mp_impl->is_null = false;
	mp_impl->is_string = true;
	return *this;
}

mysql_var & mysql_var::operator=(int const & value)
{
	*mp_impl = impl();
	mp_impl->integer = value;
	mp_impl->is_null = false;
	mp_impl->is_integer = true;
	return *this;
}

mysql_var & mysql_var::operator=(bool const & value)
{
	*mp_impl = impl();
	mp_impl->boolean = value;
	mp_impl->is_null = false;
	mp_impl->is_boolean = true;
	return *this;
}

bool mysql_var::operator ==(std::nullptr_t) const
{
	return mp_impl->is_null;
}

mysql_var::operator bool() const
{
	return !mp_impl->is_null
		&& !(is_string() && mp_impl->string.empty())
		&& !(is_boolean() && !mp_impl->boolean)
		&& !(is_integer() && !mp_impl->integer)
		;
}

#include <stdexcept>

std::string const & mysql_var::string() const
{
	if(!is_string())
		throw std::runtime_error("Value is not a string");

	return mp_impl->string;
}

bool const mysql_var::is_string() const
{
	return mp_impl->is_string;
}

int const mysql_var::integer() const
{
	if(!is_integer())
		throw std::runtime_error("Value is not an integer");

	return mp_impl->integer;
}

bool const mysql_var::is_integer() const
{
	return mp_impl->is_integer;
}

bool const mysql_var::boolean() const
{
	if(!is_boolean())
		throw std::runtime_error("Value is not an boolean");

	return mp_impl->boolean;
}

bool const mysql_var::is_boolean() const
{
	return mp_impl->is_boolean;
}

