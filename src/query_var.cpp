
#include "query_var.hpp"
#include "memory.hpp"

#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <boost/lexical_cast.hpp>

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

		value * p_former = &former;
		value * p_latter = &latter;

		(void) p_former, p_latter; // silent compiler warnings
	}

	static bool const equals(former_type const & former, latter_type const & latter);
	static bool const greater_than(former_type const & former, latter_type const & latter);
	static bool const less_than(former_type const & former, latter_type const & latter);
};

// Generic comparision (works for same type comparision)
template <typename former_type, typename latter_type>
bool const compare_traits<former_type, latter_type>::equals(former_type const & former, latter_type const & latter)
{
	return compare_traits<value, value>::equals(former, latter)
		or (nullptr != former and nullptr != latter
				and former.operator ==(latter));
}

template <typename former_type, typename latter_type>
bool const compare_traits<former_type, latter_type>::greater_than(former_type const & former, latter_type const & latter)
{
	return compare_traits<value, value>::greater_than(former, latter)
		or (former.operator >(latter));
}

template <typename former_type, typename latter_type>
bool const compare_traits<former_type, latter_type>::less_than(former_type const & former, latter_type const & latter)
{
	return compare_traits<value, value>::less_than(former, latter)
		or (former.operator <(latter));
}

// Specialisation for base class, that only nows it's null or not
template <>
bool const compare_traits<value, value>::equals(value const & former, value const & latter)
{
	return nullptr == former and nullptr == latter;
}

template <>
bool const compare_traits<value, value>::greater_than(value const & former, value const & latter)
{
	return nullptr != former and nullptr == latter;
}

template <>
bool const compare_traits<value, value>::less_than(value const & former, value const & latter)
{
	return nullptr != former and nullptr == latter;
}

// Specialization comparison string, boolean
template <>
bool const compare_traits<string, boolean>::equals(string const & former, boolean const & latter)
{
	return compare_traits<value, value>::equals(former, latter);
}

template <>
bool const compare_traits<string, boolean>::greater_than(string const & former, boolean const & latter)
{
	return compare_traits<value, value>::greater_than(former, latter) or false;
}

template <>
bool const compare_traits<string, boolean>::less_than(string const & former, boolean const & latter)
{
	return compare_traits<value, value>::greater_than(former, latter) or true;
}

// Specialization comparison boolean, string
template <>
bool const compare_traits<boolean, string>::equals(boolean const & former, string const & latter)
{
	return compare_traits<value, value>::equals(former, latter);
}

template <>
bool const compare_traits<boolean, string>::greater_than(boolean const & former, string const & latter)
{
	return compare_traits<value, value>::greater_than(former, latter) or true;
}

template <>
bool const compare_traits<boolean, string>::less_than(boolean const & former, string const & latter)
{
	return compare_traits<value, value>::greater_than(former, latter) or false;
}

// Specialization comparison string, integer
template <>
bool const compare_traits<string, integer>::equals(string const & former, integer const & latter)
{
	return compare_traits<value, value>::equals(former, latter)
		or (nullptr != former and nullptr != latter
				and former.operator ==(boost::lexical_cast<std::string>(latter.get())));
}

template <>
bool const compare_traits<string, integer>::greater_than(string const & former, integer const & latter)
{
	return compare_traits<value, value>::greater_than(former, latter)
		or (former.operator >(boost::lexical_cast<std::string>(latter.get())));
}

template <>
bool const compare_traits<string, integer>::less_than(string const & former, integer const & latter)
{
	return compare_traits<value, value>::greater_than(former, latter)
		or (former.operator <(boost::lexical_cast<std::string>(latter.get())));
}

// Specialization comparison boolean, integer
// We cast the integer in a POD to compare to the lhs boolean
template <>
bool const compare_traits<boolean, integer>::equals(boolean const & former, integer const & latter)
{
	return compare_traits<value, value>::equals(former, latter)
		or (nullptr != former and nullptr != latter
				and former.operator ==(bool(latter.get())));
}

template <>
bool const compare_traits<boolean, integer>::greater_than(boolean const & former, integer const & latter)
{
	return compare_traits<value, value>::greater_than(former, latter)
		or (former.operator >(bool(latter.get())));
}

template <>
bool const compare_traits<boolean, integer>::less_than(boolean const & former, integer const & latter)
{
	return compare_traits<value, value>::greater_than(former, latter)
		or (former.operator <(bool(latter.get())));
}

//
template<typename previous_clause_type>
struct comparator
	: const_visitor
{
	previous_clause_type const & m_former;
	bool m_truth;

	explicit comparator(previous_clause_type const & lhs)
		: m_former(lhs)
		, m_truth(false)
	{ }

	explicit comparator(value const & lhs)
		: m_former(dynamic_cast<previous_clause_type const &>(lhs))
		, m_truth(false)
	{ }

	operator bool() { return m_truth; }

	virtual ~comparator() { };

	virtual void visit(boolean const & value) = 0;
	virtual void visit(integer const & value) = 0;
	virtual void visit(string const & value) = 0;
};

template<typename previous_clause_type>
struct equality
	: comparator<previous_clause_type>
{
	explicit equality(previous_clause_type const & lhs)
		: comparator<previous_clause_type> {lhs}
	{ }

	explicit equality(value const & lhs)
		: comparator<previous_clause_type> {lhs}
	{ }

	template<typename next_clause_type>
	void equals(next_clause_type const & latter)
	{
		typedef compare_traits<previous_clause_type, next_clause_type> comparator_traits;
		this->m_truth = comparator_traits::equals(this->m_former, latter);
	}

	void equals(value const & value)		{ value.accept(*this); }

	virtual ~equality() { };

	virtual void visit(boolean const & value)	{ equals<>(value); }
	virtual void visit(integer const & value)	{ equals<>(value); }
	virtual void visit(string const & value)	{ equals<>(value); }
};

template<typename previous_clause_type>
struct greater_than
	: comparator<previous_clause_type>
{
	explicit greater_than(previous_clause_type const & lhs)
		: comparator<previous_clause_type> {lhs}
	{ }

	explicit greater_than(value const & lhs)
		: comparator<previous_clause_type> {lhs}
	{ }

	template<typename next_clause_type>
	void gt(next_clause_type const & latter)
	{
		typedef compare_traits<previous_clause_type, next_clause_type> comparator_traits;
		this->m_truth = comparator_traits::greater_than(this->m_former, latter);
	}

	void is_greater_than(value const & value)		{ value.accept(*this); }

	virtual ~greater_than() { };

	virtual void visit(boolean const & value)	{ gt<>(value); }
	virtual void visit(integer const & value)	{ gt<>(value); }
	virtual void visit(string const & value)	{ gt<>(value); }
};

template<typename previous_clause_type>
struct less_than
	: comparator<previous_clause_type>
{
	explicit less_than(previous_clause_type const & lhs)
		: comparator<previous_clause_type> {lhs}
	{ }

	explicit less_than(value const & lhs)
		: comparator<previous_clause_type> {lhs}
	{ }

	template<typename next_clause_type>
	void lt(next_clause_type const & latter)
	{
		typedef compare_traits<previous_clause_type, next_clause_type> comparator_traits;
		this->m_truth = comparator_traits::less_than(this->m_former, latter);
	}

	void is_less_than(value const & value)		{ value.accept(*this); }

	virtual ~less_than() { };

	virtual void visit(boolean const & value)	{ lt<>(value); }
	virtual void visit(integer const & value)	{ lt<>(value); }
	virtual void visit(string const & value)	{ lt<>(value); }
};

namespace comparison {

////////////////////////////////////////////////////////////////////////////////
// equality ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool operator ==(string const & lhs, webpp::query::value const & rhs)
{
	auto c = equality<string>(lhs);
	c.equals(rhs);
	return c;
}

bool operator ==(boolean const & lhs, webpp::query::value const & rhs)
{
	auto c = equality<boolean>(lhs);
	c.equals(rhs);
	return c;
}

bool operator ==(std::string const & lhs, webpp::query::value const & rhs)
{
	auto const converted = webpp::query::string {lhs};
	return operator ==(converted, rhs);
}

bool operator ==(bool const & lhs, webpp::query::value const & rhs)
{
	auto const converted = webpp::query::boolean {lhs};
	return operator ==(converted, rhs);
}

////////////////////////////////////////////////////////////////////////////////
// greater than ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool operator >(string const & lhs, webpp::query::value const & rhs)
{
	auto c = greater_than<string>(lhs);
	c.is_greater_than(rhs);
	return c;
}

bool operator >(boolean const & lhs, webpp::query::value const & rhs)
{
	auto c = greater_than<boolean>(lhs);
	c.is_greater_than(rhs);
	return c;
}

bool operator >(std::string const & lhs, webpp::query::value const & rhs)
{
	auto const converted = webpp::query::string {lhs};
	return operator >(converted, rhs);
}

bool operator >(bool const & lhs, webpp::query::value const & rhs)
{
	auto const converted = webpp::query::boolean {lhs};
	return operator >(converted, rhs);
}

////////////////////////////////////////////////////////////////////////////////
// less than ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool operator <(string const & lhs, webpp::query::value const & rhs)
{
	auto c = less_than<string>(lhs);
	c.is_less_than(rhs);
	return c;
}

bool operator <(boolean const & lhs, webpp::query::value const & rhs)
{
	auto c = less_than<boolean>(lhs);
	c.is_less_than(rhs);
	return c;
}

bool operator <(std::string const & lhs, webpp::query::value const & rhs)
{
	auto const converted = webpp::query::string {lhs};
	return operator <(converted, rhs);
}

bool operator <(bool const & lhs, webpp::query::value const & rhs)
{
	auto const converted = webpp::query::boolean {lhs};
	return operator <(converted, rhs);
}

} // namespace comparison

////////////////////////////////////////////////////////////////////////////////
// class value ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct value::impl
{
	impl() : is_null(true) {}

	bool is_null;
};

value::value()
	: mp_impl {std::make_unique<impl>()}
{
}

value::value(value const & copied)
	: mp_impl{std::make_unique<impl>(*copied.mp_impl)}
{
}

value::value(std::nullptr_t)
	: value()
{
}

value::~value()
{
}

void value::set_not_null()
{
	mp_impl->is_null = false;
}

void value::set_null()
{
	mp_impl->is_null = true;
}

value & value::operator =(value const & copied)
{
	auto p_impl = std::make_unique<impl>(*copied.mp_impl);
	std::swap(p_impl, mp_impl);
	return *this;
}

value & value::operator =(std::nullptr_t)
{
	set_null();
	return *this;
}

bool value::operator ==(std::nullptr_t) const
{
	return mp_impl->is_null;
}

bool value::operator <(std::nullptr_t) const
{
	return false;
}

bool value::operator >(std::nullptr_t) const
{
	return nullptr != *this;
}

////////////////////////////////////////////////////////////////////////////////
// class string ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
string::string()
	: value {}, m_value {}
{
}

string::string(string const & copied)
	: value {copied}, m_value {copied.m_value}
{
}

string::string(std::string const & copied)
	: value {}, m_value {copied}
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
	return nullptr == rhs
		or *this == rhs.m_value;
}

bool string::operator < (string const & rhs) const
{
	return nullptr == *this
		or nullptr != rhs
		or *this < rhs.m_value;
}

bool string::operator > (string const & rhs) const
{
	return nullptr != *this
		and *this > rhs.m_value;
}

bool string::operator ==(std::string const & rhs) const
{
	return nullptr != *this and m_value == rhs;
}

bool string::operator < (std::string const & rhs) const
{
	return nullptr == *this or m_value < rhs;
}

bool string::operator > (std::string const & rhs) const
{
	return nullptr != *this and m_value > rhs;
}

void string::accept(visitor & v)
{
	v.visit(*this);
}

void string::accept(const_visitor & v) const
{
	v.visit(*this);
}

void string::clone(std::shared_ptr<value> & p_v) const
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
	: value {}, m_value()
{
}

integer::integer(integer const & copied)
	: value{copied}, m_value(copied.m_value)
{
}

integer::integer(std::string const & copied)
	: integer(std::atoi(copied.c_str()))
{
}

integer::integer(long long const copied)
	: value {}, m_value(copied)
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

bool integer::operator < (integer const & rhs) const
{
	return nullptr == *this and nullptr != rhs
		and *this < rhs.m_value;
}

bool integer::operator > (integer const & rhs) const
{
	return nullptr != *this
		and *this > rhs.m_value;
}

bool integer::operator ==(integer const & rhs) const
{
	return (nullptr == *this and nullptr == rhs)
		or *this == rhs.m_value;
}
bool integer::operator < (long long const rhs) const
{
	return nullptr == *this
		or m_value < rhs;
}

bool integer::operator > (long long const rhs) const
{
	return nullptr != *this
		or m_value > rhs;
}

void integer::accept(visitor & v)
{
	v.visit(*this);
}

void integer::accept(const_visitor & v) const
{
	v.visit(*this);
}

void integer::clone(std::shared_ptr<value> & p_v) const
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
	: value {}, m_value()
{
}

boolean::boolean(boolean const & copied)
	:  value {copied}, m_value(copied.m_value)
{
}

boolean::boolean(bool const copied)
	: value {}, m_value(copied)
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

bool boolean::operator < (boolean const & rhs) const
{
	return nullptr == *this
		or *this > rhs.m_value;
}

bool boolean::operator > (boolean const & rhs) const
{
	return nullptr != *this
		or *this > rhs.m_value;
}

bool boolean::operator ==(boolean const & rhs) const
{
	return (nullptr != *this and nullptr != rhs)
		or *this == rhs.m_value;
}

bool boolean::operator < (bool const rhs) const
{
	return nullptr == *this
		or m_value < rhs;
}

bool boolean::operator > (bool const rhs) const
{
	return nullptr != *this
		or m_value > rhs;
}

void boolean::accept(visitor & v)
{
	v.visit(*this);
}

void boolean::accept(const_visitor & v) const
{
	v.visit(*this);
}

void boolean::clone(std::shared_ptr<value> & p_v) const
{
	p_v = std::make_unique<boolean>(*this);
}

boolean::~boolean()
{
}

} } // namespace webpp::query
