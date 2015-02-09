
#include "json.hpp"
#include "memory.hpp"

#include <istream>
#include <cstring>

namespace webpp { namespace json {

void skip(char & c, std::istream & in)
{
	if(in.eof())
		throw "Unexpected EOS";

	in.get(c);
}

void skip_dirt(char & c, std::istream & in)
{
	if(!std::isspace(c))
		skip(c, in);
}

void skip_spaces(char & c, std::istream & in)
{
	while(std::isspace(c))
		skip(c, in);
}

visitor::~visitor()	{ }
value::~value()		{ }
object::~object()	{ }
array::~array()		{ }
string::~string()	{ }
number::~number()	{ }
boolean::~boolean()	{ }
null::~null()		{ }

void object::accept(visitor const & v) const	{ v.visit(*this); }
void array::accept(visitor const & v) const		{ v.visit(*this); }
void string::accept(visitor const & v) const	{ v.visit(*this); }
void number::accept(visitor const & v) const	{ v.visit(*this); }
void boolean::accept(visitor const & v) const	{ v.visit(*this); }
void null::accept(visitor const & v) const		{ v.visit(*this); }

void parse(std::unique_ptr<value> & p_value, std::istream & in)
{
	std::unique_ptr<value> p;

	char c;
	skip(c, in);
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

void parse(std::unique_ptr<value> & p_value, char & c, std::istream & in)
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
		std::unique_ptr<string> p_string;
		build(p_string);
		p_string->parse(c, in);
		p_value = std::move(p_string);
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
	else if('0' == c or !isdigit(c) or '-' == c)
	{
		std::unique_ptr<value> p_number {std::make_unique<number>()};
		p_number->parse(c, in);
		std::swap(p_number, p_value);
	}
	else
		throw "Malformed";
}

void dump(std::ostream & out, webpp::json::value const & node)
{
	print printer{out};
	node.accept(printer);
}

template <typename value_type>
void build(std::unique_ptr<value_type> & p_node)
{
	p_node = std::move(std::make_unique<value_type>());
}

template void build<string>(std::unique_ptr<string> & p_node);
template void build<array>(std::unique_ptr<array> & p_node);
template void build<object>(std::unique_ptr<object> & p_node);
template void build<number>(std::unique_ptr<number> & p_node);
template void build<null>(std::unique_ptr<null> & p_node);
template void build<boolean>(std::unique_ptr<boolean> & p_node);

void build(std::unique_ptr<string> & p_node, std::string const value)
{
	std::remove_reference<decltype(p_node)>::type p_new;
	build(p_new);
	p_new->set(value);
	std::swap(p_new, p_node);
}

void build(std::unique_ptr<boolean> & p_node, bool const value)
{
	std::remove_reference<decltype(p_node)>::type p_new;
	build(p_new);
	p_new->set(value);
	std::swap(p_new, p_node);
}

void build(std::unique_ptr<number> & p_node, std::string const value)
{
	std::remove_reference<decltype(p_node)>::type p_new;
	build(p_new);
	p_new->set(value);
	std::swap(p_new, p_node);
}

bool object::first_condition(char c)
{
	return '{' == c;
}

void object::parse(char & c, std::istream & in)
{
	if(!first_condition(c))
		throw "Malformed";

	do
	{
		skip_dirt(c, in);
		skip_spaces(c, in);

		if('}' == c)
		{
			skip(c, in);
			break;
		}

		auto p_string = std::make_unique<string>();
		p_string->parse(c, in);

		skip_spaces(c, in);

		if(':' != c)
			throw "Malformed";

		skip(c, in);
		skip_spaces(c, in);

		{
			std::unique_ptr<value> p_value;

			webpp::json::parse(p_value, c, in);
			add_property(p_string->value(), p_value);
		}

		skip_spaces(c, in);

		if('}' == c)
		{
			skip(c, in);
			break;
		}

		if(',' != c)
			throw "Malformed";

	} while(!in.eof());
}

void object::add_property(std::string const key, std::unique_ptr<value> & p_node)
{
	m_properties.insert(std::make_pair(key, std::move(p_node)));
}

void add_property(object & self, std::string const key, std::unique_ptr<value> & p_node)
{
	self.add_property(key, p_node);
}

void add_property(object & self, std::string const key, std::unique_ptr<object> & p_object)
{
	std::unique_ptr<value> p_node {std::move(p_object)};
	self.add_property(key, p_node);
}

void add_property(object & self, std::string const key, std::unique_ptr<array> & p_array)
{
	std::unique_ptr<value> p_node {std::move(p_array)};
	self.add_property(key, p_node);
}

void add_property(object & self, std::string const key, bool const node_value)
{
	std::unique_ptr<json::boolean> p_leaf;
	build(p_leaf, node_value);
	std::unique_ptr<value> p_node {std::move(p_leaf)};
	add_property(self, key, p_node);
}

void add_property(object & self, std::string const key, std::string const node_value)
{
	std::unique_ptr<string> p_leaf;
	build(p_leaf, node_value);
	std::unique_ptr<value> p_node {std::move(p_leaf)};
	add_property(self, key, p_node);
}

void add_property(object & self, std::string const key, std::nullptr_t const )
{
	std::unique_ptr<null> p_leaf;
	build(p_leaf);
	std::unique_ptr<value> p_node {std::move(p_leaf)};
	add_property(self, key, p_node);
}

bool array::first_condition(char c)
{
	return '[' == c;
}

void array::parse(char & c, std::istream & in)
{
	if(!first_condition(c))
		throw "Malformed";

	do
	{
		skip_dirt(c, in);
		skip_spaces(c, in);

		if(']' == c)
		{
			skip(c, in);
			break;
		}

		{
			std::unique_ptr<value> p_value;

			webpp::json::parse(p_value, c, in);
			m_values.push_back(std::move(p_value));
		}

		skip_spaces(c, in);

		if(']' == c)
		{
			skip(c, in);
			break;
		}

		if(',' != c)
			throw "Malformed";

	} while(!in.eof());
}

void array::add(std::unique_ptr<value> & p_node)
{
	m_values.push_back(std::move(p_node));
}

void array::add(size_t index, std::unique_ptr<value> & p_node)
{
	std::shared_ptr<null> p_null {std::make_shared<null>()};
	if(index >= m_values.size())
		std::fill_n(std::back_inserter(m_values), 1 + index - m_values.size(), p_null);
	m_values[index] = std::move(p_node);
}

void add(array & self, std::unique_ptr<value> & p_node)
{
	self.add(p_node);
}

void add(array & self, size_t index, std::unique_ptr<value> & p_node)
{
	self.add(index, p_node);
}

/*
void add(array & self, std::unique_ptr<object> & p_object);
void add(array & self, std::unique_ptr<array> & p_array);
void add(array & self, bool const value);
void add(array & self, std::string const value);
void add(array & self, std::nullptr_t const );
*/

void add(array & self, array const array_value)
{
	std::unique_ptr<value> p_array {std::make_unique<array>(array_value)};
	self.add(p_array);
}

void add(array & self, size_t index, array const array_value)
{
	std::unique_ptr<value> p_array {std::make_unique<array>(array_value)};
	self.add(index, p_array);
}

void add(array & self, size_t index, int const number_value)
{
	auto p_number = std::make_unique<number>();
	p_number->set(number_value);
	std::unique_ptr<value> p_value {std::move(p_number)};
	self.add(index, p_value);
}

bool number::first_condition(char c)
{
	return ('0' != c and isdigit(c)) or '-' == c;
}

void number::parse(char & c, std::istream & in)
{
	std::string value;

	if(!first_condition(c))
		throw "Malformed";

	bool has_sign = false;
	bool has_decimal = false;
	bool has_exponent = false;
	bool has_sign_exponent = false;
	bool has_decimal_exponent = false;

	if('-' == c)
		has_sign = true;

	do
	{
		value += c;
		skip(c, in);

		if('+' == c or '-' == c)
		{
			if(has_exponent)
			{
				if(has_sign_exponent)
					throw "Malformed";
				else
				{
					has_sign_exponent = true;
					continue;
				}
			}
		}

		if('+' == c)
		{
			if(has_sign)
				throw "Malformed";
			else
			{
				has_sign = true;
				continue;
			}
		}

		if('.' == c)
		{
			if(has_decimal)
				if(has_exponent)
					if(has_decimal_exponent)
						throw "Malformed";
					else
						has_decimal_exponent = true;
				else
					throw "Malformed";
			else
			{
				has_decimal = true;
				continue;
			}
		}

		if('e' == c or 'E' == c)
		{
			if(has_exponent)
				throw "Malformed";
			else
			{
				has_exponent = true;
				continue;
			}
		}

		if(!isdigit(c))
			break;

	} while(!in.eof());

	m_value = value;
}

bool boolean::first_condition(char c)
{
	return 't' == c or 'f' == c;
}

void boolean::parse(char & c, std::istream & in)
{
	if(!first_condition(c))
		throw "Malformed";

	if('t' == c)
	{
		char rue[4] = "\0\0\0";
		in.get(rue, 4);

		if(0 != std::strcmp("rue", rue))
			throw "Malformed";

		m_value = true;
	}
	else if('f' == c)
	{
		char alse[5] = "\0\0\0\0";
		in.get(alse, 5);

		if(0 != std::strcmp("alse", alse))
			throw "Malformed";

		m_value = false;
	}
	else
		throw "Malformed";

	skip_dirt(c, in);
}

bool string::first_condition(char c)
{
	return '"' == c;
}

void string::parse(char & c, std::istream & in)
{
	std::string value;

	if(!first_condition(c))
		throw "Malformed";

	do
	{
		skip(c, in);

		if('"' == c)
		{
			skip(c, in);
			break;
		}

		value += c;

	} while(!in.eof());

	m_value = value;
}

bool null::first_condition(char c)
{
	return 'n' == c;
}

void null::parse(char & c, std::istream & in)
{
	if(!first_condition(c))
		throw "Malformed";

	char ull[4] = "\0\0\0";
	in.get(ull, 4);

	if(0 != strcmp("ull", ull))
		throw "Malformed";

	skip_dirt(c, in);
}

struct print::impl
{
	explicit impl(std::ostream & out) : m_out(out), m_depth(0) { };
	std::ostream &	m_out;
	size_t			m_depth;
};

print::print(std::ostream & out)
	: mp_impl {std::make_unique<impl>(out)}
{
}

print::~print()
{
}

inline std::string const indent(size_t level)
{
	return std::string(level - 1, '\t');
}

void print::visit(webpp::json::array const & node) const
{
	bool first = true;
	++mp_impl->m_depth;
	mp_impl->m_out << "[ ";
	for(auto const p_child: node.values())
	{
		if(first)
			first = false;
		else
			mp_impl->m_out << "\n" << indent(mp_impl->m_depth) << ", ";

		p_child->accept(*this);
	}
	mp_impl->m_out << "\n" << indent(mp_impl->m_depth) << "]";
	--mp_impl->m_depth;
}

void print::visit(webpp::json::object const & node) const
{
	bool first = true;
	++mp_impl->m_depth;
	mp_impl->m_out << "{ ";
	for(auto const child: node.properties())
	{
		if(first)
			first = false;
		else
			mp_impl->m_out << "\n" << indent(mp_impl->m_depth) << ", ";

		mp_impl->m_out << "\"" << child.first << "\": ";
		child.second->accept(*this);

	}
	mp_impl->m_out << "\n" << indent(mp_impl->m_depth) << "}";
	--mp_impl->m_depth;
}

void print::visit(webpp::json::string const & node) const
{
	mp_impl->m_out << "\"" << node.value() << "\"";
}

void print::visit(webpp::json::number const & node) const
{
	mp_impl->m_out << node.value();
}

void print::visit(webpp::json::null const & node) const
{
	mp_impl->m_out << "null";
}

void print::visit(webpp::json::boolean const & node) const
{
	mp_impl->m_out << (node.value() ? "true" : "false");
}

} } // namespace webpp::json

