#include "query.hpp"
#include "query_clause.hpp"

namespace webpp {
namespace query {
namespace clause {

// base ///////////////////////////////////////////////////////////////////////
base::base()
{
}

base::~base()
{
}

// select /////////////////////////////////////////////////////////////////////
select::select()
{
}

select::~select()
{
}

void select::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<select>(*this);
	std::swap(p_cloned, p_clonee);
}

void select::accept(visitor & visitor)
{
	visitor.visit(*this);
}

// fields /////////////////////////////////////////////////////////////////////
fields::fields(std::vector<std::string> const field_list, bool parenthesis)
	: m_field_list {field_list}
	, m_parenthesis(parenthesis)
{ }

fields::~fields() {}

void fields::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<fields>(*this);
	std::swap(p_cloned, p_clonee);
}

std::vector<std::string> const & fields::field_list() const
{
	return m_field_list;
}

bool const fields::parenthesis() const
{
	return m_parenthesis;
}

void fields::accept(visitor & v)
{
	v.visit(*this);
}

// insert /////////////////////////////////////////////////////////////////////
insert::insert(std::string table_name, std::vector<std::string> const field_list)
	: m_table_name {table_name}
	, m_field_list {field_list}
{ }

insert::~insert() {}

void insert::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<insert>(*this);
	std::swap(p_cloned, p_clonee);
}

std::string const & insert::table_name() const
{
	return m_table_name;
}

std::vector<std::string> const & insert::field_list() const
{
	return m_field_list;
}

void insert::accept(visitor & v)
{
	v.visit(*this);
}

// values /////////////////////////////////////////////////////////////////////
values::values(std::vector<std::vector<std::string>> const value_list)
	: m_value_list {value_list}
{ }

values::~values() {}

void values::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<values>(*this);
	std::swap(p_cloned, p_clonee);
}

std::vector<std::vector<std::string>> const & values::value_list() const
{
	return m_value_list;
}

void values::accept(visitor & v)
{
	v.visit(*this);
}

// from ///////////////////////////////////////////////////////////////////////
from::from(std::string table_name)
	: m_table_name {table_name}
{ }

from::~from() {}

std::string const & from::table_name() const
{
	return m_table_name;
}

void from::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<from>(*this);
	std::swap(p_cloned, p_clonee);
}

void from::accept(visitor & v)
{
	v.visit(*this);
}

// where //////////////////////////////////////////////////////////////////////
where::where(std::string lhs, std::string rhs)
	: m_lhs {lhs}, m_rhs {rhs}
{ }

where::~where() {}

std::string const & where::lhs() const
{
	return m_lhs;
}

std::string const & where::rhs() const
{
	return m_rhs;
}

void where::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<where>(*this);
	std::swap(p_cloned, p_clonee);
}

void where::accept(visitor & v)
{
	v.visit(*this);
}

// and_ ///////////////////////////////////////////////////////////////////////
and_::and_(std::string lhs, std::string rhs)
	: m_lhs {lhs}, m_rhs {rhs}
{ }

and_::~and_() {}

std::string const & and_::lhs() const
{
	return m_lhs;
}

std::string const & and_::rhs() const
{
	return m_rhs;
}

void and_::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<and_>(*this);
	std::swap(p_cloned, p_clonee);
}

void and_::accept(visitor & v)
{
	v.visit(*this);
}

// update /////////////////////////////////////////////////////////////////////
update::update(std::string table)
	: m_table {table}
{
}

std::string const & update::table_name() const
{
	return m_table;
}

void update::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<update>(*this);
	std::swap(p_cloned, p_clonee);
}

void update::accept(visitor & v)
{
	v.visit(*this);
}

update::~update()
{
}

// set ////////////////////////////////////////////////////////////////////////
set::set(std::map<std::string, std::string> field_list)
	: m_field_list { field_list }
{
}

set::set(std::string field_name, std::string value_name)
	: set {{{field_name, value_name}}}
{
}

std::map<std::string, std::string> const & set::field_list()
{
	return m_field_list;
}

void set::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<set>(*this);
	std::swap(p_cloned, p_clonee);
}

void set::accept(visitor & v)
{
	v.visit(*this);
}

set::~set()
{
}

}}} // namespace webpp::query::clause

