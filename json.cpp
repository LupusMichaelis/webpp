
#include "json.hpp"
#include "memory.hpp"

#include <istream>
#include <cstring>

namespace webpp { namespace json {

struct parser::impl
{
};

parser::parser()
	: mp_impl {std::make_unique<parser::impl>()}
{
}

parser::~parser()
{
}

parser::value::~value() { }
parser::object::~object() { }
parser::array::~array() { }
parser::string::~string() { }
parser::number::~number() { }
parser::boolean::~boolean() { }
parser::null::~null() { }
parser::root::~root() { }

void parser::parse(std::unique_ptr<root> & p_root, std::istream & in)
{
	auto p_out = std::make_unique<std::remove_reference<decltype(p_root)>::type::element_type>();
	p_out->parse(0, in);
	std::swap(p_root, p_out);
}

void parser::root::parse(char c, std::istream & in)
{
	std::unique_ptr<value> p_state;

	if(0 == c)
		in.get(c);

	if('[' == c)
	{
		std::unique_ptr<value> p_array_state {std::make_unique<array>()};
		std::swap(p_state, p_array_state);
	}
	else if('{' == c)
	{
		std::unique_ptr<value> p_object_state {std::make_unique<object>()};
		std::swap(p_state, p_object_state);
	}
	else
		throw "Malformed";

	p_state->parse(c, in);
	mp_node.reset(p_state.release());
}

void parser::value::parse(char c, std::istream & in)
{
	if(0 == c)
		in.get(c);

	while(std::isspace(c))
		in.get(c);

	if('{' == c)
	{
		std::unique_ptr<value> p_object_state {std::make_unique<object>()};
		p_object_state->parse(c, in);
	}
	else if('"' == c)
	{
		std::unique_ptr<value> p_string_state {std::make_unique<string>()};
		p_string_state->parse(c, in);
	}
	else if('[' == c)
	{
		std::unique_ptr<value> p_array_state {std::make_unique<array>()};
		p_array_state->parse(c, in);
	}
	else if('-' == c or std::isdigit(c))
	{
		std::unique_ptr<value> p_digit_state {std::make_unique<number>()};
		p_digit_state->parse(c, in);
	}
	else if('t' == c or 'f' == c)
	{
		std::unique_ptr<value> p_boolean_state {std::make_unique<boolean>()};
		p_boolean_state->parse(c, in);
	}
	else if('n' == c)
	{
		std::unique_ptr<value> p_null_state {std::make_unique<null>()};
		p_null_state->parse(c, in);
	}
	else
		throw "Malformed";
}

void fast_forward_space(char & c, std::istream & in)
{
	do
	{
		in.get(c);
		if(!std::isspace(c))
			break;

	} while(!in.eof());

}

// XXX relooop properties
void parser::object::parse(char c, std::istream & in)
{
	if(0 == c)
		in.get(c);

	if('{' != c)
		throw "Malformed";

	do
	{
		fast_forward_space(c, in);

		if('}' == c)
			break;

		auto p_string_state = std::make_unique<string>();
		p_string_state->parse(c, in);

		fast_forward_space(c, in);

		if(':' != c)
			throw "Malformed";

		in.get(c);

		std::shared_ptr<value> p_value_state {std::make_shared<string>()}; // XXX
		p_value_state->parse(0, in);

		m_properties.insert(std::make_pair(p_string_state->m_value, p_value_state));

		fast_forward_space(c, in);

		if('}' == c)
			break;

		if(',' != c)
			throw "Malformed";

	} while(!in.eof());
}

void parser::array::parse(char c, std::istream & in)
{
	std::unique_ptr<value> p_array_state {std::make_unique<array>()};
	p_array_state->parse(0, in);

}

void parser::number::parse(char c, std::istream & in)
{
	m_value += c;
}

void parser::boolean::parse(char c, std::istream & in)
{
	m_value = 't' == c;
}

void parser::string::parse(char c, std::istream & in)
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

void parser::null::parse(char c, std::istream & in)
{
	char ull[4];
	in.get(ull, 3);

	if(0 != strcmp("ull", ull))
		throw "Malformed";
}

} } // namespace webpp::json

