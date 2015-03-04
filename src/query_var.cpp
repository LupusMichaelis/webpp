
#include "query_var.hpp"
#include "memory.hpp"

#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace webpp { namespace query {

visitor::~visitor()
{
}

const_visitor::~const_visitor()
{
}

using namespace comparison;

////////////////////////////////////////////////////////////////////////////////
// Comparison //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <typename former_type, typename latter_type>
struct compare_traits
{
	static void constraint()
	{
		former_type former;
		latter_type latter;

		var * p_former = &former;
		var * p_latter = &latter;

		(void) p_former, p_latter; // silent compiler warnings
	}

	static bool const equals(former_type const & former, latter_type const & latter)
	{
		return (nullptr == former and nullptr == latter)
			or (nullptr != former and nullptr != latter
					and former.operator ==(latter));
	}

	/*
	static bool const greater_than(former_type const & former, latter_type const & latter)
	{
		return nullptr != former and nullptr == latter;
	}

	static bool const lower_than(former_type const & former, latter_type const & latter)
	{
		return nullptr == former and nullptr != latter;
	}
	*/
};

template<>
struct compare_traits<string, boolean>
{
	static bool const equals(string const & former, boolean const & latter)
	{
		return nullptr == former and nullptr == latter;
	}
};

template<>
struct compare_traits<string, integer>
{
	static bool const equals(string const & former, integer const & latter)
	{
		return nullptr == former and nullptr == latter;
	}
};

template<>
struct compare_traits<integer, boolean>
{
	static bool const equals(integer const & former, boolean const & latter)
	{
		return nullptr == former and nullptr == latter;
	}
};

template<>
struct compare_traits<integer, string>
{
	static bool const equals(integer const & former, string const & latter)
	{
		return nullptr == former and nullptr == latter;
	}
};

template<>
struct compare_traits<boolean, integer>
{
	static bool const equals(boolean const & former, integer const & latter)
	{
		return nullptr == former and nullptr == latter;
	}
};

template<>
struct compare_traits<boolean, string>
{
	static bool const equals(boolean const & former, string const & latter)
	{
		return nullptr == former and nullptr == latter;
	}
};

/*
template<>
struct compare_traits<boolean, boolean>
{
	static bool const equals(boolean const & former, boolean const & latter)
	{
		return former.get() == latter.get();
	}

	static bool const greater_than(boolean const & former, boolean const & latter)
	{
		return former.get() > latter.get();
	}

	static bool const lower_than(boolean const & former, boolean const & latter)
	{
		return former.get() < latter.get();
	}
};
*/

template<typename previous_clause_type>
struct comparator
	: public const_visitor
{
	previous_clause_type const & m_former;
	bool m_equals;

	explicit comparator(previous_clause_type const & lhs)
		: m_former(lhs)
		, m_equals(false)
	{ }

	~comparator() { };

	template<typename next_clause_type>
	void equals(next_clause_type const & latter)
	{
		typedef compare_traits<previous_clause_type, next_clause_type> comparator_traits;
		m_equals = comparator_traits::equals(m_former, latter);
	}

	virtual void equals(var const & value)			{ value.accept(*this); }
	virtual void visit(boolean const & value)		{ equals<>(value); }
	virtual void visit(integer const & value)		{ equals<>(value); }
	virtual void visit(string const & value)		{ equals<>(value); }

	operator bool() { return m_equals; }
};

namespace comparison {

bool operator ==(string const & lhs, webpp::query::var const & rhs)
{
	auto c = comparator<string>(lhs);
	c.equals(rhs);
	return c;
}

bool operator ==(boolean const & lhs, webpp::query::var const & rhs)
{
	auto c = comparator<boolean>(lhs);
	c.equals(rhs);
	return c;
}

bool operator ==(std::string const & lhs, webpp::query::var const & rhs)
{
	auto const converted = webpp::query::string {lhs};
	return operator ==(converted, rhs);
}

bool operator ==(bool const & lhs, webpp::query::var const & rhs)
{
	auto const converted = webpp::query::boolean {lhs};
	return operator ==(converted, rhs);
}

} // namespace comparison

////////////////////////////////////////////////////////////////////////////////
// class var ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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

void var::set_null()
{
	mp_impl->is_null = true;
}

var & var::operator =(var const & copied)
{
	auto p_impl = std::make_unique<impl>(*copied.mp_impl);
	std::swap(p_impl, mp_impl);
	return *this;
}

var & var::operator =(std::nullptr_t)
{
	set_null();
	return *this;
}

bool var::operator ==(std::nullptr_t) const
{
	return mp_impl->is_null;
}

/*
var::operator bool() const
{
	return !mp_impl->is_null;
}
*/

////////////////////////////////////////////////////////////////////////////////
// class string ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
string::string()
	: var {}, m_value {}
{
}

string::string(string const & copied)
	: var {copied}, m_value {copied.m_value}
{
}

string::string(std::string const & copied)
	: var {}, m_value {copied}
{
	set_not_null();
}

string & string::operator=(string const & copied)
{
	if(nullptr == copied)
		*this = nullptr;
	else
		set(copied.get());

	return *this;
}

string & string::operator=(std::string const & copied)
{
	set(copied);

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

bool string::operator ==(string const & rhs) const
{
	return (nullptr == *this and nullptr == rhs)
		or *this == rhs.m_value;
}

bool string::operator ==(std::string const & rhs) const
{
	return nullptr != *this and m_value == rhs;
}

void string::accept(visitor & v)
{
	v.visit(*this);
}

void string::accept(const_visitor & v) const
{
	v.visit(*this);
}

void string::clone(std::shared_ptr<var> & p_v) const
{
	p_v = std::make_unique<string>(*this);
}

string::~string()
{
}

////////////////////////////////////////////////////////////////////////////////
// class integer ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
integer::integer()
	: var {}, m_value()
{
}

integer::integer(integer const & copied)
	: var{copied}, m_value(copied.m_value)
{
}

integer::integer(std::string const & copied)
	: integer(std::atoi(copied.c_str()))
{
}

integer::integer(long long const copied)
	: var {}, m_value(copied)
{
}

integer & integer::operator=(integer const & copied)
{
	m_value = copied.m_value;
	return *this;
}


long long const & integer::get() const
{
	return m_value;
}

void integer::set(long long const new_value)
{
	set_not_null();
	m_value = new_value;
}

bool integer::operator ==(long long const rhs) const
{
	return nullptr != *this or (m_value and rhs);
}

bool integer::operator ==(integer const & rhs) const
{
	return (nullptr == *this and nullptr == rhs)
		or *this == rhs.m_value;
}

void integer::accept(visitor & v)
{
	v.visit(*this);
}

void integer::accept(const_visitor & v) const
{
	v.visit(*this);
}

void integer::clone(std::shared_ptr<var> & p_v) const
{
	p_v = std::make_unique<integer>(*this);
}

integer::~integer()
{
}

////////////////////////////////////////////////////////////////////////////////
// class boolean ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
boolean::boolean()
	: var {}, m_value()
{
}

boolean::boolean(boolean const & copied)
	:  var {copied}, m_value(copied.m_value)
{
}

boolean::boolean(bool const copied)
	: var {}, m_value(copied)
{
	set_not_null();
}

boolean & boolean::operator=(boolean const & copied)
{
	m_value = copied.m_value;
	return *this;
}

boolean & boolean::operator =(bool const new_value)
{
	set_not_null();
	m_value = new_value;

	return *this;
}

bool boolean::operator ==(bool const rhs) const
{
	return nullptr != *this and m_value == rhs;
}

bool boolean::operator ==(boolean const & rhs) const
{
	return (nullptr != *this and nullptr != rhs)
		or *this == rhs.m_value;
}

void boolean::accept(visitor & v)
{
	v.visit(*this);
}

void boolean::accept(const_visitor & v) const
{
	v.visit(*this);
}

void boolean::clone(std::shared_ptr<var> & p_v) const
{
	p_v = std::make_unique<boolean>(*this);
}

boolean::~boolean()
{
}

} } // namespace webpp::query
