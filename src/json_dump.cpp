#include "json_dump.hpp"

namespace webpp { namespace json {

////////////////////////////////////////////////////////////////////////////////
// Dump JSON ///////////////////////////////////////////////////////////////////
void dump(std::ostream & out, webpp::json::value const & node)
{
	print printer{out};
	node.accept(printer);
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
