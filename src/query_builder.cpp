
#include "query_builder.hpp"
#include "query.hpp"

#include "algorithm.hpp"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace webpp {
namespace query {

////////////////////////////////////////////////////////////////////////////////
// class builder ///////////////////////////////////////////////////////////////
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
	std::for_each(mp_impl->m_clause_list, std::ref(q));
	return q;
}

builder builder::select() const
{
	builder copy{*this};
	return copy.select({});
}

builder builder::select(std::vector<schema::field> field_list) const
{
	builder copy{*this};
	return copy.select(field_list);
}

builder builder::insert(schema::table table_name) const
{
	builder copy{*this};
	return copy.insert(table_name);
}

builder builder::insert(schema::table table_name, std::vector<schema::field> field_list) const
{
	builder copy{*this};
	return copy.insert(table_name, field_list);
}

builder builder::replace(schema::table table_name) const
{
	builder copy{*this};
	return copy.replace(table_name);
}

builder builder::replace(schema::table table_name, std::vector<schema::field> field_list) const
{
	builder copy{*this};
	return copy.replace(table_name, field_list);
}

builder builder::update(schema::table table_name) const
{
	builder copy{*this};
	return copy.update(table_name);
}

builder builder::from(schema::table table_name) const
{
	builder copy{*this};
	return copy.from(table_name);
}

builder builder::where(schema::field field, std::shared_ptr<value> p_field_value) const
{
	builder copy{*this};
	return copy.where(field, p_field_value);
}

builder builder::and_(schema::field field, std::shared_ptr<value> p_field_value) const
{
	builder copy{*this};
	return copy.and_(field, p_field_value);
}

builder builder::values(std::vector<std::vector<std::shared_ptr<value>>> tuple_list) const
{
	builder copy{*this};
	return copy.values(tuple_list);
}

builder builder::set(std::map<schema::field, std::shared_ptr<value>> field_value_pair_list) const
{
	builder copy{*this};
	return copy.set(field_value_pair_list);
}

builder & builder::from(schema::table table)
{
	auto p_clause = std::make_shared<clause::from>(table);
	push_clause(p_clause);

	return *this;
}

builder & builder::where(schema::field field, std::shared_ptr<value> p_value)
{
	auto p_clause = std::make_shared<clause::where>(field, p_value);
	push_clause(p_clause);

	return *this;
}

builder & builder::and_(schema::field field, std::shared_ptr<value> p_value)
{
	auto p_clause = std::make_shared<clause::and_>(field, p_value);
	push_clause(p_clause);

	return *this;
}

builder & builder::values(std::vector<std::vector<std::shared_ptr<value>>> tuple_list)
{
	auto p_clause = std::make_shared<clause::values>(tuple_list);
	push_clause(p_clause);

	return *this;
}

builder & builder::set(std::map<schema::field, std::shared_ptr<value>> field_value_pair_list)
{
	auto p_clause = std::make_shared<clause::set>(field_value_pair_list);
	push_clause(p_clause);

	return *this;
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

builder builder::fields(std::vector<schema::field> field_list, bool parenthesis/* = false*/) const
{
	builder copy{*this};
	return copy.fields(field_list, parenthesis);
}

builder & builder::fields(std::vector<schema::field> field_list, bool parenthesis/* = false*/)
{
	auto p_clause = std::make_shared<clause::fields>(field_list, parenthesis);
	mp_impl->m_clause_list.push_back(p_clause);

	return *this;
}

builder & builder::select(std::vector<schema::field> field_list)
{
	auto p_clause = std::make_shared<clause::select>();
	push_clause(p_clause);

	fields(field_list);

	return *this;
}

builder & builder::replace(schema::table table_name)
{
	auto p_clause = std::make_shared<clause::replace>(table_name);
	push_clause(p_clause);

	return *this;
}

builder & builder::replace(schema::table table_name, std::vector<schema::field> field_list)
{
	auto p_clause = std::make_shared<clause::replace>(table_name, field_list);
	push_clause(p_clause);
	fields(field_list, true);

	return *this;
}

builder & builder::insert(schema::table table_name)
{
	auto p_clause = std::make_shared<clause::insert>(table_name);
	push_clause(p_clause);

	return *this;
}

builder & builder::insert(schema::table table_name, std::vector<schema::field> field_list)
{
	auto p_clause = std::make_shared<clause::insert>(table_name, field_list);
	push_clause(p_clause);
	fields(field_list, true);

	return *this;
}

builder & builder::update(schema::table table_name)
{
	auto p_clause = std::make_shared<clause::update>(table_name);
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

////////////////////////////////////////////////////////////////////////////////
// class simple_builder ////////////////////////////////////////////////////////

simple_builder::simple_builder(escaper const & e)
	: builder {e}
{
}

simple_builder::simple_builder(simple_builder const & copied)
	: builder {copied}
{
}

simple_builder::simple_builder(builder const & copied)
	: builder {copied}
{
}

simple_builder::~simple_builder()
{
};

simple_builder simple_builder::select() const
{
	return builder::select();
}

simple_builder simple_builder::select(std::vector<std::string> field_list) const
{
	std::vector<schema::field> fields;
	std::transform(field_list.cbegin(), field_list.cend(), std::back_inserter(fields)
			, [] (std::string const & name) -> schema::field { return schema::field{name}; }
			);

	simple_builder copy{*this};
	return copy.select(field_list);
}

simple_builder simple_builder::insert(std::string table_name, std::vector<std::string> field_list) const
{
	simple_builder copy{*this};
	return copy.insert(table_name, field_list);
}

simple_builder simple_builder::insert(std::string table_name) const
{
	simple_builder copy{*this};
	return copy.insert(table_name);
}

simple_builder simple_builder::replace(std::string table_name) const
{
	simple_builder copy{*this};
	return copy.replace(table_name);
}

simple_builder simple_builder::replace(std::string table_name, std::vector<std::string> field_list) const
{
	simple_builder copy{*this};
	return copy.replace(table_name, field_list);
}

simple_builder simple_builder::update(std::string table_name) const
{
	simple_builder copy{*this};
	return copy.update(table_name);
}

simple_builder simple_builder::from(std::string table_name) const
{
	simple_builder copy{*this};
	return copy.from(table_name);
}

simple_builder simple_builder::where(std::string field_name, std::string field_value) const
{
	simple_builder copy{*this};
	return copy.where(field_name, field_value);
}

simple_builder simple_builder::and_(std::string field_name, std::string field_value) const
{
	simple_builder copy{*this};
	return copy.and_(field_name, field_value);
}

simple_builder simple_builder::values(std::vector<std::vector<std::string>> tuple_list) const
{
	simple_builder copy{*this};
	return copy.values(tuple_list);
}

simple_builder simple_builder::set(std::map<std::string, std::string> field_value_pair_list) const
{
	simple_builder copy{*this};
	return copy.set(field_value_pair_list);
}

simple_builder & simple_builder::from(std::string table_name)
{
	schema::table table{table_name};

	return static_cast<simple_builder &>(builder::from(table));
}

simple_builder & simple_builder::where(std::string field_name, std::string field_value)
{
	schema::field field{field_name};
	std::shared_ptr<value> p_value = std::make_shared<string>(field_value);

	return static_cast<simple_builder &>(builder::where(field, p_value));
}

simple_builder & simple_builder::and_(std::string field_name, std::string field_value)
{
	schema::field field{field_name};
	std::shared_ptr<value> p_value = std::make_shared<string>(field_value);


	return static_cast<simple_builder &>(builder::and_(field, p_value));
}

simple_builder & simple_builder::values(std::vector<std::vector<std::string>> tuple_list)
{
	std::vector<std::vector<std::shared_ptr<value>>> tuples;
	std::transform(tuple_list.cbegin(), tuple_list.cend(), std::back_inserter(tuples)
		, [] (std::vector<std::string> const & values)
			-> std::vector<std::shared_ptr<value>>
			{
				std::vector<std::shared_ptr<value>> tuple;
				std::transform(values.cbegin(), values.cend(), std::back_inserter(tuple)
					, [] (std::string const & v)
						-> std::shared_ptr<value>
						{
							return std::make_shared<string>(v);
						}
					);

				return tuple;
			}
		);

	return static_cast<simple_builder &>(builder::values(tuples));
}

simple_builder & simple_builder::set(std::map<std::string, std::string> field_value_pair_list)
{
	std::map<schema::field, std::shared_ptr<value>> field_value_pairs;
	std::transform(field_value_pair_list.cbegin(), field_value_pair_list.cend()
			, std::inserter(field_value_pairs, field_value_pairs.begin())
			, [] (std::pair<std::string, std::string> const & field_value_pair)
				-> std::pair<schema::field, std::shared_ptr<value>>
				{
					return {schema::field{field_value_pair.first}
						, std::make_shared<string>(field_value_pair.second)};
				}
			);

	return static_cast<simple_builder &>(builder::set(field_value_pairs));
}

simple_builder & simple_builder::select(std::vector<std::string> field_list)
{
	std::vector<schema::field> fields;
	std::transform(field_list.cbegin(), field_list.cend(), std::back_inserter(fields)
			, [] (std::string const & name) -> schema::field { return schema::field{name}; }
			);

	return static_cast<simple_builder &>(builder::select(fields));
}

simple_builder & simple_builder::replace(std::string table_name)
{
	schema::table table {table_name};

	return static_cast<simple_builder &>(builder::replace(table));
}

simple_builder & simple_builder::replace(std::string table_name
		, std::vector<std::string> field_list)
{
	schema::table table {table_name};
	std::vector<schema::field> fields;
	std::transform(field_list.cbegin(), field_list.cend(), std::back_inserter(fields)
			, [] (std::string const & name) -> schema::field { return schema::field{name}; }
			);

	return static_cast<simple_builder &>(builder::replace(table, fields));
}

simple_builder & simple_builder::insert(std::string table_name)
{
	schema::table table {table_name};

	return static_cast<simple_builder &>(builder::insert(table));
}

simple_builder & simple_builder::insert(std::string table_name, std::vector<std::string> field_list)
{
	schema::table table {table_name};
	std::vector<schema::field> fields;
	std::transform(field_list.cbegin(), field_list.cend(), std::back_inserter(fields)
			, [] (std::string const & name) -> schema::field { return schema::field{name}; }
			);

	return static_cast<simple_builder &>(builder::insert(table, fields));
}

simple_builder & simple_builder::update(std::string table_name)
{
	schema::table table {table_name};

	return static_cast<simple_builder &>(builder::update(table));
}

}} // namespace webpp::query
