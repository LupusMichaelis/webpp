
#include "mysql_var.hpp"

#include <string>

namespace webpp { namespace mysql {

struct var::impl
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

var::var()
	: mp_impl {std::make_unique<impl>()}
{
}

var::var(var const & copied)
	: mp_impl{std::make_unique<impl>(*copied.mp_impl)}
{
}

var::~var()
{
}

var & var::operator=(var const & copied)
{
	auto p_impl = std::make_unique<impl>(*copied.mp_impl);
	std::swap(p_impl, mp_impl);
	return *this;
}

var & var::operator=(std::nullptr_t const & null)
{
	*mp_impl = impl();
	return *this;
}

var & var::operator=(std::string const & value)
{
	*mp_impl = impl();
	mp_impl->string = value;
	mp_impl->is_null = false;
	mp_impl->is_string = true;
	return *this;
}

var & var::operator=(int const & value)
{
	*mp_impl = impl();
	mp_impl->integer = value;
	mp_impl->is_null = false;
	mp_impl->is_integer = true;
	return *this;
}

var & var::operator=(bool const & value)
{
	*mp_impl = impl();
	mp_impl->boolean = value;
	mp_impl->is_null = false;
	mp_impl->is_boolean = true;
	return *this;
}

bool var::operator ==(std::nullptr_t) const
{
	return mp_impl->is_null;
}

var::operator bool() const
{
	return !mp_impl->is_null
		&& !(is_string() && mp_impl->string.empty())
		&& !(is_boolean() && !mp_impl->boolean)
		&& !(is_integer() && !mp_impl->integer)
		;
}

#include <stdexcept>

std::string const & var::string() const
{
	if(!is_string())
		throw std::runtime_error("Value is not a string");

	return mp_impl->string;
}

bool const var::is_string() const
{
	return mp_impl->is_string;
}

int const var::integer() const
{
	if(!is_integer())
		throw std::runtime_error("Value is not an integer");

	return mp_impl->integer;
}

bool const var::is_integer() const
{
	return mp_impl->is_integer;
}

bool const var::boolean() const
{
	if(!is_boolean())
		throw std::runtime_error("Value is not an boolean");

	return mp_impl->boolean;
}

bool const var::is_boolean() const
{
	return mp_impl->is_boolean;
}

} } // namespace webpp::mysql
