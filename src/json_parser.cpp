
#include "json_parser.hpp"
#include "json.hpp"

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

void parse(std::unique_ptr<json::value> & p_value, std::istream & in)
{
	parser engine;
	engine.parse(p_value, in);
}

struct parser::impl
{
	std::vector<std::shared_ptr<value>> m_prototypes;

	impl(parser & parser): m_prototypes
		{ std::make_shared<array>(parser)
		, std::make_shared<object>(parser)
		, std::make_shared<string>(parser)
		, std::make_shared<boolean>(parser)
		, std::make_shared<number>(parser)
		, std::make_shared<null>(parser)
		} { }
};

parser::parser():
	mp_impl { std::make_unique<impl>(*this) }
{
}

parser::value::~value()		{ }
parser::object::~object()	{ }
parser::array::~array()		{ }
parser::string::~string()	{ }
parser::number::~number()	{ }
parser::boolean::~boolean()	{ }
parser::null::~null()		{ }

struct parser::value::impl
{
	parser & m_parser;
	std::unique_ptr<json::value> mp_value;

	impl(parser & engine)
		: m_parser (engine)
		//, mp_value {nullptr}
	{
	};

};

parser::value::value(parser & engine)
	: mp_impl {std::make_unique<impl>(engine)}
{
}

parser::object::object(parser & engine)		: value {engine} { }
parser::array::array(parser & engine)		: value {engine} { }
parser::string::string(parser & engine)		: value {engine} { }
parser::number::number(parser & engine)		: value {engine} { }
parser::boolean::boolean(parser & engine)	: value {engine} { }
parser::null::null(parser & engine)			: value {engine} { }

parser & parser::value::ref_parser() const
{
	return mp_impl->m_parser;
}

void parser::value::assign_value(std::unique_ptr<json::value> & p_value)
{
	std::swap(p_value, mp_impl->mp_value);
}

json::value const & parser::value::ref_value() const
{
	return *mp_impl->mp_value;
}

parser::value::value(value const & value)
	: mp_impl {std::make_unique<impl>(value.mp_impl->m_parser)}
{
}

void parser::parse(std::unique_ptr<json::value> & p_value, std::istream & in)
{
	std::unique_ptr<value> p_parser_value;
	parse(p_parser_value, in);
	p_parser_value->ref_value().clone(p_value);
}

void parser::parse(std::unique_ptr<value> & p_value, std::istream & in)
{
	char c;
	skip(c, in);
	skip_spaces(c, in);

	std::unique_ptr<value> p_root;
	for(auto & p_prototype: mp_impl->m_prototypes)
		if(p_prototype->first_condition(c))
			if(p_prototype->is_top_level())
			{
				p_prototype->create(p_root);
				break;
			}

	if(!p_root)
		throw "Malformed";

	p_root->parse(c, in);
	p_value = std::move(p_root);
}

void parser::parse(std::unique_ptr<value> & p_value, char & c, std::istream & in)
{
	skip_spaces(c, in);

	std::unique_ptr<value> p_node;
	for(auto & p_prototype: mp_impl->m_prototypes)
		if(p_prototype->first_condition(c))
		{
			p_prototype->create(p_node);
			break;
		}

	if(!p_node)
		throw "Malformed";

	p_node->parse(c, in);
	p_value = std::move(p_node);
}

void parser::object::parse(char & c, std::istream & in)
{
	if(!first_condition(c))
		throw "Malformed";

	auto p_node = std::make_unique<json::object>();

	do
	{
		skip_dirt(c, in);
		skip_spaces(c, in);

		if('}' == c)
		{
			skip(c, in);
			break;
		}

		auto p_string = std::make_unique<string>(ref_parser());
		p_string->parse(c, in);
		auto r_string = dynamic_cast<json::string const &>(p_string->ref_value());

		skip_spaces(c, in);

		if(':' != c)
			throw "Malformed";

		skip(c, in);
		skip_spaces(c, in);

		{
			std::unique_ptr<value> p_value;

			ref_parser().parse(p_value, c, in);

			std::unique_ptr<json::value> p_leaf;
			p_value->ref_value().clone(p_leaf);
			p_node->add_property(r_string.get(), p_leaf);
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

	std::unique_ptr<json::value> p_value {std::move(p_node)};
	assign_value(p_value);
}

void parser::array::parse(char & c, std::istream & in)
{
	if(!first_condition(c))
		throw "Malformed";

	auto p_node = std::make_unique<json::array>();
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

			ref_parser().parse(p_value, c, in);

			std::unique_ptr<json::value> p_leaf;

			p_value->ref_value().clone(p_leaf);
			add(*p_node, p_leaf);
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

	std::unique_ptr<json::value> p_value {std::move(p_node)};
	assign_value(p_value);
}

void parser::number::parse(char & c, std::istream & in)
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

	json::number the_number;
	the_number.set(value);
	std::unique_ptr<json::value> p_value = std::make_unique<json::number>(the_number);
	assign_value(p_value);
}

void parser::boolean::parse(char & c, std::istream & in)
{
	if(!first_condition(c))
		throw "Malformed";

	json::boolean the_boolean;
	if('t' == c)
	{
		char rue[4] = "\0\0\0";
		in.get(rue, 4);

		if(0 != std::strcmp("rue", rue))
			throw "Malformed";

		the_boolean.set(true);
	}
	else if('f' == c)
	{
		char alse[5] = "\0\0\0\0";
		in.get(alse, 5);

		if(0 != std::strcmp("alse", alse))
			throw "Malformed";

		the_boolean.set(false);
	}
	else
		throw "Malformed";

	skip_dirt(c, in);

	std::unique_ptr<json::value> p_value = std::make_unique<json::boolean>(the_boolean);
	assign_value(p_value);
}

void parser::string::parse(char & c, std::istream & in)
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

	json::string the_string;
	the_string.set(value);
	std::unique_ptr<json::value> p_value = std::make_unique<json::string>(the_string);
	assign_value(p_value);
}

void parser::null::parse(char & c, std::istream & in)
{
	if(!first_condition(c))
		throw "Malformed";

	char ull[4] = "\0\0\0";
	in.get(ull, 4);

	if(0 != strcmp("ull", ull))
		throw "Malformed";

	skip_dirt(c, in);

	std::unique_ptr<json::value> p_value = std::make_unique<json::null>();
	assign_value(p_value);
}

bool parser::object::first_condition(char c)
{
	return '{' == c;
}

bool parser::object::is_top_level()
{
	return true;
}

void parser::object::create(std::unique_ptr<value> & p_value) const
{
	std::remove_reference<decltype(p_value)>::type p_object {std::make_unique<object>(ref_parser())};
	std::swap(p_value, p_object);
}

bool parser::array::first_condition(char c)
{
	return '[' == c;
}

bool parser::array::is_top_level()
{
	return true;
}

void parser::array::create(std::unique_ptr<value> & p_value) const
{
	std::remove_reference<decltype(p_value)>::type p_array {std::make_unique<array>(ref_parser())};
	std::swap(p_value, p_array);
}

bool parser::number::first_condition(char c)
{
	return ('0' != c and isdigit(c)) or '-' == c;
}

bool parser::number::is_top_level()
{
	return false;
}

void parser::number::create(std::unique_ptr<value> & p_value) const
{
	std::remove_reference<decltype(p_value)>::type p_number {std::make_unique<number>(ref_parser())};
	std::swap(p_value, p_number);
}

bool parser::boolean::first_condition(char c)
{
	return 't' == c or 'f' == c;
}

bool parser::boolean::is_top_level()
{
	return false;
}

void parser::boolean::create(std::unique_ptr<value> & p_value) const
{
	std::remove_reference<decltype(p_value)>::type p_boolean {std::make_unique<boolean>(ref_parser())};
	std::swap(p_value, p_boolean);
}

bool parser::string::first_condition(char c)
{
	return '"' == c;
}

bool parser::string::is_top_level()
{
	return false;
}

void parser::string::create(std::unique_ptr<value> & p_value) const
{
	std::remove_reference<decltype(p_value)>::type p_string {std::make_unique<string>(ref_parser())};
	std::swap(p_value, p_string);
}

bool parser::null::first_condition(char c)
{
	return 'n' == c;
}

bool parser::null::is_top_level()
{
	return false;
}

void parser::null::create(std::unique_ptr<value> & p_value) const
{
	std::remove_reference<decltype(p_value)>::type p_null {std::make_unique<null>(ref_parser())};
	std::swap(p_value, p_null);
}

} } // namespace webpp::json
