
#include "json.hpp"
#include "memory.hpp"

#include <istream>
#include <cstring>

namespace webpp { namespace json {

void skip_spaces(char & c, std::istream & in)
{
	if(0 == c or std::isspace(c))
	do
	{
		in.get(c);
		if(!std::isspace(c))
			break;

	} while(!in.eof());
}

visitor::~visitor() { }
value::~value() { }
object::~object() { }
array::~array() { }
string::~string() { }
number::~number() { }
boolean::~boolean() { }
null::~null() { }

void object::visit(visitor const & v) const		{ v.visit(*this); }
void array::visit(visitor const & v) const		{ v.visit(*this); }
void string::visit(visitor const & v) const		{ v.visit(*this); }
void number::visit(visitor const & v) const		{ v.visit(*this); }
void boolean::visit(visitor const & v) const	{ v.visit(*this); }
void null::visit(visitor const & v) const		{ v.visit(*this); }

void start(std::unique_ptr<value> & p_value, std::istream & in)
{
	std::unique_ptr<value> p;

	char c = 0;
	skip_spaces(c, in);

	if('[' == c)
	{
		std::unique_ptr<value> p_array {std::make_unique<array>()};
		std::swap(p, p_array);
	}
	else if('{' == c)
	{
		std::unique_ptr<value> p_object {std::make_unique<object>()};
		std::swap(p, p_object);
	}
	else
		throw "Malformed";

	p->parse(c, in);
	p_value.reset(p.release()); // XXX leak memory possible?
}

void parse(std::unique_ptr<value> & p_value, char c, std::istream & in)
{
	skip_spaces(c, in);

	if('{' == c)
	{
		std::unique_ptr<value> p_object {std::make_unique<object>()};
		p_object->parse(c, in);
		std::swap(p_object, p_value);
	}
	else if('"' == c)
	{
		std::unique_ptr<value> p_string {std::make_unique<string>()};
		p_string->parse(c, in);
		std::swap(p_string, p_value);
	}
	else if('[' == c)
	{
		std::unique_ptr<value> p_array {std::make_unique<array>()};
		p_array->parse(c, in);
		std::swap(p_array, p_value);
	}
	else if('-' == c or std::isdigit(c))
	{
		std::unique_ptr<value> p_digit {std::make_unique<number>()};
		p_digit->parse(c, in);
		std::swap(p_digit, p_value);
	}
	else if('t' == c or 'f' == c)
	{
		std::unique_ptr<value> p_boolean {std::make_unique<boolean>()};
		p_boolean->parse(c, in);
		std::swap(p_boolean, p_value);
	}
	else if('n' == c)
	{
		std::unique_ptr<value> p_null {std::make_unique<null>()};
		p_null->parse(c, in);
		std::swap(p_null, p_value);
	}
	else
		throw "Malformed";
}

void object::parse(char c, std::istream & in)
{
	if(0 == c)
		in.get(c);

	if('{' != c)
		throw "Malformed";

	do
	{
		in.get(c);
		skip_spaces(c, in);

		if('}' == c)
			break;

		auto p_string = std::make_unique<string>();
		p_string->parse(c, in);

		in.get(c);
		skip_spaces(c, in);

		if(':' != c)
			throw "Malformed";

		in.get(c);
		skip_spaces(c, in);

		{
			std::unique_ptr<value> p_value;

			webpp::json::parse(p_value, c, in);
			m_properties.insert(std::make_pair(p_string->value(), std::move(p_value)));
		}

		in.get(c);
		skip_spaces(c, in);

		if('}' == c)
			break;

		if(',' != c)
			throw "Malformed";

	} while(!in.eof());
}

void array::parse(char c, std::istream & in)
{
	std::unique_ptr<value> p_array {std::make_unique<array>()};

	if(0 == c)
		in.get(c);

	if('[' != c)
		throw "Malformed";

	do
	{
		in.get(c);
		skip_spaces(c, in);

		if(']' == c)
			break;

		{
			std::unique_ptr<value> p_value;

			webpp::json::parse(p_value, c, in);
			m_values.push_back(std::move(p_value));
		}

		in.get(c);
		skip_spaces(c, in);

		if(']' == c)
			break;

		if(',' != c)
			throw "Malformed";

	} while(!in.eof());
}

void number::parse(char c, std::istream & in)
{
	m_value += c;
}

void boolean::parse(char c, std::istream & in)
{
	if('t' == c)
	{
		char rue[4] = "\0\0\0";
		in.get(rue, 4);

		if(0 != strcmp("rue", rue))
			throw "Malformed";

		m_value = true;
	}
	else if('f' == c)
	{
		char alse[5] = "\0\0\0\0";
		in.get(alse, 5);

		if(0 != strcmp("alse", alse))
			throw "Malformed";

		m_value = false;
	}
	else
		throw "Malformed";
}

void string::parse(char c, std::istream & in)
{
	std::string value;
	if(0 == c)
		in.get(c);

	if('"' != c)
		throw "Malformed";

	do
	{
		in.get(c);

		if('"' == c)
			break;

		value += c;

	} while(!in.eof());

	m_value = value;
}

void null::parse(char c, std::istream & in)
{
	char ull[4] = "\0\0\0";
	in.get(ull, 4);

	if(0 != strcmp("ull", ull))
		throw "Malformed";
}

} } // namespace webpp::json

