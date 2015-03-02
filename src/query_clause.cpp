#include "query.hpp"
#include "query_clause.hpp"
#include "query_var.hpp"
#include "query_schema.hpp"

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
fields::fields(std::vector<schema::field> field_list, bool parenthesis)
	: m_field_list {field_list}
	, m_parenthesis(parenthesis)
{ }

fields::~fields() {}

void fields::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<fields>(*this);
	std::swap(p_cloned, p_clonee);
}

std::vector<schema::field> const & fields::field_list() const
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
insert::insert(schema::table table)
	: m_table {table}
{
}

insert::insert(schema::table table, std::vector<schema::field> field_list)
	: m_table {table}
	, m_field_list {field_list}
{
}

insert::~insert() {}

void insert::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<insert>(*this);
	std::swap(p_cloned, p_clonee);
}

schema::table const & insert::table() const
{
	return m_table;
}

std::vector<schema::field> const & insert::field_list() const
{
	return m_field_list;
}

void insert::accept(visitor & v)
{
	v.visit(*this);
}

// replace ////////////////////////////////////////////////////////////////////
replace:: replace(schema::table table)
	: m_table {table}
	, m_field_list {}
{
}

replace::replace(schema::table table, std::vector<schema::field> field_list)
	: m_table {table}
	, m_field_list {field_list}
{
}

replace::~replace() {}

void replace::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<replace>(*this);
	std::swap(p_cloned, p_clonee);
}

schema::table const & replace::table() const
{
	return m_table;
}

std::vector<schema::field> const & replace::field_list() const
{
	return m_field_list;
}

void replace::accept(visitor & v)
{
	v.visit(*this);
}

// values /////////////////////////////////////////////////////////////////////
values::values(std::vector<std::vector<std::shared_ptr<var>>> value_list)
	: m_value_list {value_list}
{ }

values::~values() {}

void values::clone(std::unique_ptr<base> & p_cloned)
{
	std::unique_ptr<base> p_clonee = std::make_unique<values>(*this);
	std::swap(p_cloned, p_clonee);
}

std::vector<std::vector<std::shared_ptr<var>>> const & values::value_list() const
{
	return m_value_list;
}

void values::accept(visitor & v)
{
	v.visit(*this);
}

// from ///////////////////////////////////////////////////////////////////////
from::from(schema::table table)
	: m_table {table}
{ }

from::~from() {}

schema::table const & from::table() const
{
	return m_table;
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
where::where(schema::field lhs, std::shared_ptr<var> p_rhs)
	: m_lhs {lhs}
	, mp_rhs {p_rhs}
{ }

where::~where() {}

schema::field const & where::lhs() const
{
	return m_lhs;
}

var const & where::rhs() const
{
	return *mp_rhs;
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
and_::~and_() {}

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
update::update(schema::table table)
	: m_table {table}
{
}

schema::table const & update::table() const
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
set::set(std::map<schema::field, std::shared_ptr<var>> field_value_pair_list)
	: m_field_list { field_value_pair_list }
{
}

set::set(schema::field field, std::shared_ptr<var> value)
	: set {{{field, value}}}
{
}

std::map<schema::field, std::shared_ptr<var>> const & set::field_value_pair_list()
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

