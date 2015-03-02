
#include "model.hpp"
#include "mysql_connection.hpp"
#include "mysql_result.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

#include "query.hpp"
#include "query_builder.hpp"

namespace webpp {

class mysql_to_string:
	public query::visitor
{
	std::string m_yielded;

	void yield_null()
	{
		m_yielded = "NULL";
	}

	public:
		void convert(std::string & out, query::var & var)
		{
			m_yielded = "";
			var.accept(*this);
			out = m_yielded;
		}

		virtual void visit(query::boolean & v)
		{
			if(nullptr == v)
				yield_null();
			else if(v)
				m_yielded = "TRUE";
			else
				m_yielded = "FALSE";
		}

		virtual void visit(query::integer & v)
		{
			if(nullptr == v)
				yield_null();
			else
				m_yielded = v.get();
		}

		virtual void visit(query::string & v)
		{
			if(nullptr == v)
				yield_null();
			else
				// TODO escape!
				m_yielded = "\"" + v.get() + "\"";
		}

		virtual ~mysql_to_string()
		{
		}
};

struct model::impl
{
	impl()
		: m_query_builder {query::escaper()}
	{ }

	std::shared_ptr<mysql::connection> p_connection;
	query::simple_builder const m_query_builder;
};

model::model()
	: mp_impl {std::make_unique<model::impl>()}
{
}

model::~model()
{
}

query::simple_builder const & model::query_builder() const
{
	return mp_impl->m_query_builder;
}

void model::connection(std::shared_ptr<mysql::connection> p_connection)
{
	mp_impl->p_connection = p_connection;
}

void model::get_by_criterias
	( std::unique_ptr<field_list_type> & p_fields
	, std::unique_ptr<row_list_type> & p_rows
	, std::string table_name
	, criteria_list_type criterias
	)
{
	webpp::query::query query = query_builder()
		.select()
		.from(table_name)
		.builder::where(criterias.cbegin()->first, criterias.cbegin()->second)
		;

	std::unique_ptr<mysql::result> p_result;
	mp_impl->p_connection->query(p_result, query);

	std::unique_ptr<mysql::result::field_list_type> p_field_list;
	p_result->fields(p_field_list);
	p_fields = std::make_unique<field_list_type>();
	std::transform(p_field_list->cbegin()
		, p_field_list->cend()
		, std::back_inserter(*p_fields)
		, [] (mysql::result::field_list_type::value_type const & p_field)
			-> std::string
			{ return p_field->name(); }
		);

	p_result->rows(p_rows);
}

bool model::update(std::string table_name
		, criteria_list_type set_criterias
		, criteria_list_type where_criterias
		)
{
	webpp::query::query query = query_builder()
		.update(table_name)
		.builder::where(set_criterias.cbegin()->first, set_criterias.cbegin()->second)
		.builder::where(where_criterias.cbegin()->first, where_criterias.cbegin()->second)
		;

	std::unique_ptr<mysql::result> p_result;
	mp_impl->p_connection->query(p_result, query);

	return p_result->updated_rows() > 0;
}

bool model::insert(std::string table_name, row_list_type rows)
{
	webpp::query::query query = query_builder()
		.insert(table_name)
		.builder::values(rows)
		;

	std::unique_ptr<mysql::result> p_result;
	mp_impl->p_connection->query(p_result, query);

	return p_result->updated_rows() > 0;
}

bool model::replace(std::string table_name, row_type row)
{
	webpp::query::query query = query_builder()
		.replace(table_name)
		.builder::values(std::vector<row_type>{row})
		;

	std::unique_ptr<mysql::result> p_result;
	mp_impl->p_connection->query(p_result, query);

	return p_result->updated_rows() > 0;
}

} // webpp
