
#include "query_builder.hpp"
#include "query.hpp"

#include <algorithm>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace webpp {
namespace query {

struct builder::impl
{
	escaper const & m_escaper;
	std::vector<std::shared_ptr<clause::base>> m_clause_list;

	impl(escaper const & e)
		: m_escaper(e)
		, m_clause_list()
	{ }

	impl(impl const & copied)
		: m_escaper(copied.m_escaper)
		, m_clause_list()
	{
		std::transform(copied.m_clause_list.cbegin()
			, copied.m_clause_list.cend()
			, std::back_inserter(m_clause_list)
			, [] (std::shared_ptr<clause::base> const & p_value)
				-> std::shared_ptr<clause::base>
				{
					std::unique_ptr<clause::base> p_cloned;
					p_value->clone(p_cloned);
					return std::move(p_cloned);
				}
			);
	};
};

builder::builder(builder const & copied)
	: mp_impl {std::make_unique<impl>(*copied.mp_impl)}
{
}

builder::builder(escaper const & e)
	: mp_impl {std::make_unique<impl>(e)}
{
}

builder::~builder()
{
};

builder::operator query() const
{
	query q{mp_impl->m_escaper};
	std::for_each(mp_impl->m_clause_list.cbegin()
		, mp_impl->m_clause_list.cend()
		, std::ref(q));
	return q;
}

builder builder::fields(std::vector<std::string> field_list, bool parenthesis/* = false*/) const
{
	builder copy{*this};
	return copy.fields(field_list, parenthesis);
}

builder & builder::fields(std::vector<std::string> field_list, bool parenthesis/* = false*/)
{
	auto p_clause = std::make_shared<clause::fields>(field_list, parenthesis);
	mp_impl->m_clause_list.push_back(p_clause);

	return *this;
}

builder builder::select() const
{
	builder copy{*this};
	return copy.select({});
}

builder builder::select(std::vector<std::string> field_list) const
{
	builder copy{*this};
	return copy.select(field_list);
}

builder builder::insert(std::string table_name, std::vector<std::string> field_list) const
{
	builder copy{*this};
	return copy.insert(table_name, field_list);
}

builder builder::replace(std::string table_name, std::vector<std::string> field_list) const
{
	builder copy{*this};
	return copy.replace(table_name, field_list);
}

builder builder::update(std::string table_name) const
{
	builder copy{*this};
	return copy.update(table_name);
}

builder builder::from(std::string table_name) const
{
	builder copy{*this};
	return copy.from(table_name);
}

builder builder::where(std::string field_name, std::string field_value) const
{
	builder copy{*this};
	return copy.where(field_name, field_value);
}

builder builder::and_(std::string field_name, std::string field_value) const
{
	builder copy{*this};
	return copy.and_(field_name, field_value);
}

builder builder::values(std::vector<std::vector<std::string>> value_list) const
{
	builder copy{*this};
	return copy.values(value_list);
}

builder builder::set(std::map<std::string, std::string> field_list) const
{
	builder copy{*this};
	return copy.set(field_list);
}

template <typename clause_type>
void builder::verify_clause_is_allowed(clause_type & clause)
{
	if(mp_impl->m_clause_list.size())
	{
		clause::checker<clause_type> checker{clause};
		checker.check(*mp_impl->m_clause_list.back());

		if(!checker)
			throw "Boom";
	}
}

template <typename clause_type>
void builder::push_clause(std::shared_ptr<clause_type> const & p_clause)
{
	verify_clause_is_allowed<clause_type>(*p_clause);
	mp_impl->m_clause_list.push_back(p_clause);
}

builder & builder::select(std::vector<std::string> field_list)
{
	auto p_clause = std::make_shared<clause::select>();
	push_clause(p_clause);

	fields(field_list);

	return *this;
}

builder & builder::replace(std::string table_name, std::vector<std::string> field_list)
{
	auto p_clause = std::make_shared<clause::replace>(table_name, field_list);
	push_clause(p_clause);
	fields(field_list, true);

	return *this;
}

builder & builder::insert(std::string table_name, std::vector<std::string> field_list)
{
	auto p_clause = std::make_shared<clause::insert>(table_name, field_list);
	push_clause(p_clause);
	fields(field_list, true);

	return *this;
}

builder & builder::update(std::string table_name)
{
	auto p_clause = std::make_shared<clause::update>(table_name);
	push_clause(p_clause);

	return *this;
}

builder & builder::from(std::string table_name)
{
	auto p_clause = std::make_shared<clause::from>(table_name);
	push_clause(p_clause);

	return *this;
}

builder & builder::where(std::string field_name, std::string field_value)
{
	auto p_clause = std::make_shared<clause::where>(field_name, field_value);
	push_clause(p_clause);

	return *this;
}

builder & builder::and_(std::string field_name, std::string field_value)
{
	auto p_clause = std::make_shared<clause::and_>(field_name, field_value);
	push_clause(p_clause);

	return *this;
}

builder & builder::values(std::vector<std::vector<std::string>> value_list)
{
	auto p_clause = std::make_shared<clause::values>(value_list);
	push_clause(p_clause);

	return *this;
}

builder & builder::set(std::map<std::string, std::string> field_list)
{
	auto p_clause = std::make_shared<clause::set>(field_list);
	push_clause(p_clause);

	return *this;
}

template
void builder::verify_clause_is_allowed<clause::select>(clause::select & clause);
template
void builder::push_clause<clause::select>(std::shared_ptr<clause::select> const & p_clause);

template
void builder::verify_clause_is_allowed<clause::fields>(clause::fields & clause);
template
void builder::push_clause<clause::fields>(std::shared_ptr<clause::fields> const & p_clause);

template
void builder::verify_clause_is_allowed<clause::insert>(clause::insert & clause);
template
void builder::push_clause<clause::insert>(std::shared_ptr<clause::insert> const & p_clause);

template
void builder::verify_clause_is_allowed<clause::from>(clause::from & clause);
template
void builder::push_clause<clause::from>(std::shared_ptr<clause::from> const & p_clause);

template
void builder::verify_clause_is_allowed<clause::where>(clause::where & clause);
template
void builder::push_clause<clause::where>(std::shared_ptr<clause::where> const & p_clause);

template
void builder::verify_clause_is_allowed<clause::and_>(clause::and_ & clause);
template
void builder::push_clause<clause::and_>(std::shared_ptr<clause::and_> const & p_clause);

template
void builder::verify_clause_is_allowed<clause::values>(clause::values & clause);
template
void builder::push_clause<clause::values>(std::shared_ptr<clause::values> const & p_clause);

template
void builder::verify_clause_is_allowed<clause::set>(clause::set & clause);
template
void builder::push_clause<clause::set>(std::shared_ptr<clause::set> const & p_clause);

template
void builder::verify_clause_is_allowed<clause::replace>(clause::replace & clause);
template
void builder::push_clause<clause::replace>(std::shared_ptr<clause::replace> const & p_clause);

}} // namespace webpp::query
