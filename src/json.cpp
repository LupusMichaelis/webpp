
#include "json.hpp"
#include "memory.hpp"

#include <istream>
#include <iterator>
#include <algorithm>

namespace webpp { namespace json {

array::array(array const & copied)
	: m_values{}
{
	std::transform(copied.m_values.cbegin()
		, copied.m_values.cend()
		, std::back_inserter(m_values)
		, [] (std::shared_ptr<value> const & p_value) -> std::shared_ptr<value> const
			{
				std::unique_ptr<value> p_clone;
				p_value->clone(p_clone);
				return std::move(p_clone);
			}
		);
}

object::object(object const & copied)
	: m_properties{}
{
	typedef decltype(m_properties)::value_type pair_type;

	std::transform(copied.m_properties.cbegin()
		, copied.m_properties.cend()
		, std::inserter(m_properties, m_properties.begin())
		, [] (pair_type const & pair) -> pair_type const
			{
				std::unique_ptr<value> p_clone;
				pair.second->clone(p_clone);
				return std::make_pair(pair.first, std::move(p_clone));
			}
		);
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

void dump(std::ostream & out, webpp::json::value const & node)
{
	print printer{out};
	node.accept(printer);
}

void object::clone(std::unique_ptr<value> & p_v) const
{
	std::unique_ptr<value> p_clone = std::make_unique<object>(*this);
	std::swap(p_v, p_clone);
}

void array::clone(std::unique_ptr<value> & p_v) const
{
	std::unique_ptr<value> p_clone = std::make_unique<array>(*this);
	std::swap(p_v, p_clone);
}

void string::clone(std::unique_ptr<value> & p_v) const
{
	std::unique_ptr<value> p_clone = std::make_unique<string>(*this);
	std::swap(p_v, p_clone);
}

void number::clone(std::unique_ptr<value> & p_v) const
{
	std::unique_ptr<value> p_clone = std::make_unique<number>(*this);
	std::swap(p_v, p_clone);
}

void boolean::clone(std::unique_ptr<value> & p_v) const
{
	std::unique_ptr<value> p_clone = std::make_unique<boolean>(*this);
	std::swap(p_v, p_clone);
}

void null::clone(std::unique_ptr<value> & p_v) const
{
	std::unique_ptr<value> p_clone = std::make_unique<null>(*this);
	std::swap(p_v, p_clone);
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

template <typename storage_type>
void build(std::unique_ptr<value> & p_node)
{
	p_node = std::move(std::make_unique<storage_type>());
}

template void build<null>(std::unique_ptr<value> & p_node);

template <typename value_type, typename native_value>
void build(std::unique_ptr<value_type> & p_node, native_value const & value)
{
	typename std::remove_reference<decltype(p_node)>::type p_new;
	build(p_new);
	p_new->set(value);
	std::swap(p_new, p_node);
}

template void build<string, std::string>(std::unique_ptr<string> & p_node, std::string const & value);
template void build<boolean, bool>(std::unique_ptr<boolean> & p_node, bool const & value);
template void build<number, std::string>(std::unique_ptr<number> & p_node, std::string const & value);

template <typename value_type, typename native_value>
void build(std::unique_ptr<value> & p_node, native_value const & value)
{
	typename std::unique_ptr<value_type> p_new;
	build(p_new, value);
	p_node = std::move(p_new);
}

template void build<string, std::string>(std::unique_ptr<value> & p_node, std::string const & value);
template void build<boolean, bool>(std::unique_ptr<value> & p_node, bool const & value);
template void build<number, std::string>(std::unique_ptr<value> & p_node, std::string const & value);
template void build<number, long long>(std::unique_ptr<value> & p_node, long long const & value);

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
	std::unique_ptr<value> p_node;
	build<boolean>(p_node, node_value);
	add_property(self, key, p_node);
}

void add_property(object & self, std::string const key, std::string const node_value)
{
	std::unique_ptr<value> p_node;
	build<string>(p_node, node_value);
	add_property(self, key, p_node);
}

void add_property(object & self, std::string const key, std::nullptr_t const )
{
	std::unique_ptr<value> p_node;
	build<null>(p_node);
	add_property(self, key, p_node);
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

void add(array & self, std::unique_ptr<object> & p_object)
{
	std::unique_ptr<value> p_value {std::move(p_object)};
	self.add(p_value);
}

void add(array & self, std::unique_ptr<array> & p_array);
void add(array & self, bool const value);
void add(array & self, std::string const value);
void add(array & self, std::nullptr_t const );

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
	std::unique_ptr<value> p_value;
	build<number>(p_value, number_value);
	self.add(index, p_value);
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
	mp_impl->m_out << "\"" << node.get() << "\"";
}

void print::visit(webpp::json::number const & node) const
{
	mp_impl->m_out << node.get();
}

void print::visit(webpp::json::null const & node) const
{
	mp_impl->m_out << "null";
}

void print::visit(webpp::json::boolean const & node) const
{
	mp_impl->m_out << (node.get() ? "true" : "false");
}

} } // namespace webpp::json

