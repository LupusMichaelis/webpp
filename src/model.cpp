
#include "model.hpp"
#include "mysql_connection.hpp"
#include "mysql_result.hpp"

#include "query.hpp"
#include "query_builder.hpp"

#include <boost/format.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace webpp::query::comparison;

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
		void convert(std::string & out, query::value & value)
		{
			m_yielded = "";
			value.accept(*this);
			out = m_yielded;
		}

		virtual void visit(query::boolean & v)
		{
			if(nullptr == v)
				yield_null();
			else if(true == v)
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
	webpp::query::simple_builder builder = query_builder()
		.select()
		.from(table_name)
		;

	if(0 < criterias.size())
		builder.builder::where(criterias.cbegin()->first, criterias.cbegin()->second) ;

	webpp::query::query query {builder};

	std::unique_ptr<mysql::result> p_result;
	mp_impl->p_connection->query(p_result, query);

	std::unique_ptr<mysql::result::field_list_type> p_field_list;
	p_result->fields(p_field_list);
	p_fields = std::make_unique<field_list_type>();
	std::transform(p_field_list->cbegin()
		, p_field_list->cend()
		, std::back_inserter(*p_fields)
		, [] (mysql::result::field_list_type::value_type const & p_field)
			-> query::schema::field
			{ return query::schema::field{p_field->name()}; }
		);

	p_result->rows(p_rows);
}

bool model::update(std::string table_name
		, field_list_type const & fields
		, row_list_type const & original_rows
		, row_list_type const & updated_rows
		, criteria_list_type where_criterias
		)
{
	bool return_value = false;

	auto builder = query_builder()
		.update(table_name)
		;

	auto & connection = *mp_impl->p_connection;

	std::for_each
		( boost::make_zip_iterator(boost::make_tuple(original_rows.cbegin(), updated_rows.cbegin()))
		, boost::make_zip_iterator(boost::make_tuple(original_rows.cend(), updated_rows.cend()))
		, [&builder, &fields, &connection, &return_value, &where_criterias]
			(const boost::tuple<row_type const & , row_type const & > tuple) -> void
			{
				auto const & original = tuple.get<0>();
				auto const & updated = tuple.get<1>();

				std::map<query::schema::field, std::shared_ptr<query::value>> set;

				for(size_t idx = 0; idx < fields.size(); ++idx)
				{
					auto field = fields.at(idx);
					auto const & original_value = original.at(idx);
					auto const & updated_value = updated.at(idx);

					//if(nullptr != updated_value && *original_value != *updated_value)
					if(nullptr != updated_value)
						set.insert({field, updated_value});
				}

				if(set.size() > 0u)
				{
					webpp::query::query query = builder
						.builder::set(set)
						.builder::where(where_criterias.cbegin()->first, where_criterias.cbegin()->second);

					std::unique_ptr<mysql::result> p_result;
					connection.query(p_result, query);

					return_value |= p_result->updated_rows() > 0;
				}
			}
		);

	return return_value;
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

} // webpp
