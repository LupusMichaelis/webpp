
#include "mysql_var.hpp"
#include "memory.hpp"

#include <string>
#include <stdexcept>
#include <iostream>

namespace webpp { namespace mysql {

visitor::~visitor()
{
}

struct printer::impl
{
	impl(std::ostream & out) : m_out(out) {};
	std::ostream & m_out;
};

printer::printer(std::ostream & out)
	: mp_impl{std::make_unique<impl>(out)}
{
}

void printer::visit(string & v)
{
	mp_impl->m_out << v.get();
}

printer:: ~printer()
{
}

struct var::impl
{
	impl() : is_null(true) {}

	bool is_null;
};

var::var()
	: mp_impl {std::make_unique<impl>()}
{
}

var::var(var const & copied)
	: mp_impl{std::make_unique<impl>(*copied.mp_impl)}
{
}

var::var(std::nullptr_t)
	: var()
{
}

var::~var()
{
}

void var::set_not_null()
{
	mp_impl->is_null = false;
}

var & var::operator =(var const & copied)
{
	auto p_impl = std::make_unique<impl>(*copied.mp_impl);
	std::swap(p_impl, mp_impl);
	return *this;
}

var & var::operator =(std::nullptr_t)
{
	mp_impl->is_null = true;
	return *this;
}

bool var::operator ==(std::nullptr_t) const
{
	return mp_impl->is_null;
}

var::operator bool() const
{
	return !mp_impl->is_null;
}

string::string()
	: m_value {}
{
}

string::string(string const & copied)
	: var {copied}, m_value {copied.m_value}
{
}

string::string(std::string const & copied)
	: var(), m_value {copied}
{
	set_not_null();
}

string::operator bool() const
{
	return var::operator bool() or !m_value.empty();
}

string & string::operator=(string const & copied)
{
	set_not_null();
	m_value = copied;

	return *this;
}

std::string const & string::get() const
{
	return m_value;
}

void string::set(std::string const & new_value)
{
	set_not_null();
	m_value = new_value;
}

bool string::operator ==(std::string const & rhs) const
{
	return nullptr != *this and m_value == rhs;
}

void string::accept(visitor & v)
{
	v.visit(*this);
}

string::~string()
{
}

} } // namespace webpp::mysql
