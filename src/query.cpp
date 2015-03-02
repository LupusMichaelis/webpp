
#include "query.hpp"
#include "query_var.hpp"

#include <algorithm>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace webpp {
namespace query {

struct query::impl
{
	escaper const & m_escaper;

	std::string m_literal;

	impl(impl const & rhs)
		: m_escaper(rhs.m_escaper)
		, m_literal(rhs.m_literal)
	{ }

	impl(escaper const & e)
		: m_escaper(e)
	{ }
};

query::query(escaper const & e)
	: mp_impl {std::make_unique<query::impl>(e)}
{
}

query::query(query const & rhs)
	: mp_impl {std::make_unique<query::impl>(*rhs.mp_impl)}
{
}

void query::operator() (std::shared_ptr<clause::base> const & p_clause)
{
	operator() (*p_clause);
}

void query::operator() (clause::base & clause)
{
	clause.accept(*this);
}

void query::visit(clause::select & clause)
{
	mp_impl->m_literal = "select";
}

void query::visit(clause::fields & clause)
{
	if(!clause.field_list().size())
	{
		mp_impl->m_literal += " *";
		return;
	}

	std::vector<std::string> field_list;
	std::transform(clause.field_list().cbegin()
		, clause.field_list().cend()
		, std::back_inserter(field_list)
		, [] (schema::field const & field)
			-> std::string const
			{
				return "`" + field.name()  + "`";
			}
		);
	auto field_list_literal = boost::algorithm::join(field_list, ", ");
	if(clause.parenthesis())
		field_list_literal = "(" + field_list_literal + ")";

	mp_impl->m_literal += " " + field_list_literal;
}


void query::visit(clause::from & clause)
{
	mp_impl->m_literal += (boost::format(" from `%s`") % clause.table().name()).str();
}

void query::visit(clause::where & clause)
{
	auto const & value = dynamic_cast<string const &>(clause.rhs());
	mp_impl->m_literal += (boost::format(" where `%s` = %s")
			% clause.lhs().name()
			% value.get()
			).str();
}

void query::visit(clause::and_ & clause)
{
	auto const & value = dynamic_cast<string const &>(clause.rhs());
	mp_impl->m_literal += (boost::format(" and `%s` = %s")
			% clause.lhs().name()
			% value.get()
			).str();
}

void query::visit(clause::insert & clause)
{
	mp_impl->m_literal = "insert `" + clause.table().name()  + "`";
}

void query::visit(clause::values & clause)
{
	std::vector<std::string> value_row_list;
	for(auto value_row: clause.value_list())
	{
		std::vector<std::string> value_list;

		std::transform(value_row.cbegin()
			, value_row.cend()
			, std::back_inserter(value_list)
			, [] (std::shared_ptr<var> const & p_var)
				-> std::string const
				{
					auto const & value = dynamic_cast<string const &>(*p_var);
					return "\"" + value.get()  + "\"";
				}
			);
		value_row_list.push_back("(" + boost::algorithm::join(value_list, ", ") + ")");
	}

	auto field_list_literal = boost::algorithm::join(value_row_list, ", ");

	mp_impl->m_literal += " values " + field_list_literal;
}

void query::visit(clause::update & clause)
{
	mp_impl->m_literal = (boost::format("update `%s`") % clause.table().name()).str();
}

void query::visit(clause::set & clause)
{
	std::vector<std::string> buffer;
	for(auto setter: clause.field_value_pair_list())
	{
		auto const & value = dynamic_cast<string const &>(*setter.second);
		buffer.push_back((boost::format("`%s` = %s")
				% setter.first.name()
				% value.get()
				).str());
	}

	mp_impl->m_literal += " set " + boost::algorithm::join(buffer, ", ");
}

void query::visit(clause::replace & clause)
{
	mp_impl->m_literal = "replace `" + clause.table().name()  + "`";
}

std::string const query::str() const
{
	return mp_impl->m_literal;
}

query::~query()
{
}

}} // namespace webpp::query
