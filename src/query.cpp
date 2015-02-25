
#include "query.hpp"

#include <algorithm>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace webpp {
namespace query {

struct query::impl
{
	escaper const & m_escaper;

	std::string m_table_name;
	std::vector<std::string> m_field_list;
	std::map<std::string, std::string> m_where_list;

	impl(impl const & rhs)
		: m_escaper(rhs.m_escaper)
		, m_table_name {rhs.m_table_name}
		, m_field_list {rhs.m_field_list}
		, m_where_list {rhs.m_where_list}
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

query query::select(std::vector<std::string> field_list) const
{
	query copy{*this};
	copy.mp_impl->m_field_list = field_list;

	return copy;
}

query query::from(std::string table_name) const
{
	query copy{*this};
	copy.mp_impl->m_table_name = table_name;

	return copy;
}

query query::where(std::string field_name, std::string field_value) const
{
	query copy{*this};
	copy.mp_impl->m_where_list.insert({field_name, field_value});

	return copy;
}

query query::and_(std::string field_name, std::string field_value) const
{
	if(!mp_impl->m_where_list.size())
		throw "";

	return where(field_name, field_value);
}

std::string const query::str() const
{
	std::vector<std::string> field_list;
	std::transform(mp_impl->m_field_list.cbegin()
		, mp_impl->m_field_list.cend()
		, std::back_inserter(field_list)
		, [] (std::string const & value) -> std::string const
			{
				return "`" + value  + "`";
			}
		);
	auto field_list_literal = boost::algorithm::join(field_list, ", ");
	std::string literal = (boost::format("select %s from `%s`")
			% field_list_literal
			% mp_impl->m_table_name).str();

			//mysql_to_string converter;
			//std::string converted;
			//converter.convert(converted, *criteria.second);
	std::vector<std::string> buffer;
	std::transform(mp_impl->m_where_list.cbegin()
			, mp_impl->m_where_list.cend()
			, std::back_inserter(buffer)
			, [] (std::pair<std::string, std::string> const & criteria) -> std::string const
				{
					return (boost::format("`%s` = %s") % criteria.first % criteria.second).str();
				}
			);

	literal += " where " + boost::algorithm::join(buffer, " and ");

	return literal;
}

query::~query()
{
}

struct builder::impl
{
	escaper m_escaper;
};

builder::builder()
	: mp_impl {std::make_unique<builder::impl>()}
{
}

query builder::select(std::vector<std::string> field_list) const
{
	query q{mp_impl->m_escaper};

	return q.select(field_list);
}

builder::~builder()
{
};

}} // namespace webpp::query
