
#include "model.hpp"
#include "mysql_connection.hpp"
#include "mysql_result.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

#include "query.hpp"

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
	std::shared_ptr<mysql::connection> p_connection;
};

model::model()
	: mp_impl {std::make_unique<model::impl>()}
{
}

model::~model()
{
}

void model::connection(std::shared_ptr<mysql::connection> p_connection)
{
	mp_impl->p_connection = p_connection;
}

static void build_where_clause(std::string & where, model::row_type criterias)
{
	if(!criterias.size())
		return;

	mysql_to_string converter;
	std::vector<std::string> buffer;
	for(auto criteria: criterias)
	{
		std::string converted;
		converter.convert(converted, *criteria.second);
		buffer.push_back((boost::format("`%s` = %s") % criteria.first % converted).str());
	}

	where = " where " + boost::algorithm::join(buffer, " and ");
}

static void build_set_clause(std::string & set, model::row_type criterias)
{
	if(!criterias.size())
		return;

	mysql_to_string converter;
	std::vector<std::string> buffer;
	for(auto criteria: criterias)
	{
		std::string converted;
		converter.convert(converted, *criteria.second);
		buffer.push_back((boost::format("`%s` = %s") % criteria.first % converted).str());
	}

	set = " set " + boost::algorithm::join(buffer, ", ");
}

static void build_field_clause(std::string & field_list, model::row_type const & row)
{
	std::vector<std::string> fields;
	for(auto & pair: row)
		fields.push_back(pair.first);

	field_list = boost::algorithm::join(fields, ", ");
}

static void build_value_clause(std::string & field_list, model::row_type const & row)
{
	mysql_to_string converter;
	std::vector<std::string> fields;
	for(auto & pair: row)
	{
		std::string converted;
		converter.convert(converted, *pair.second);
		fields.push_back(converted);
	}

	field_list = boost::algorithm::join(fields, ", ");
}

void model::get_by_criterias
	( std::unique_ptr<row_list_type> & p_rows
	, std::string table_name
	, row_type criterias
	)
{
	std::string query {(boost::format("select * from `%s`") % table_name).str()};

	std::string where;
	build_where_clause(where, criterias);
	query += where;

	std::unique_ptr<mysql::result> p_result;
	mp_impl->p_connection->query(p_result, query);
	p_result->rows(p_rows);
}

bool model::update(std::string table_name
		, row_type set_criterias
		, row_type where_criterias
		)
{
	webpp::query::query query;

	std::string query {(boost::format("update `%s`") % table_name).str()};

	{
		std::string set;
		build_set_clause(set, set_criterias);
		query += set;
	}

	{
		std::string where;
		build_where_clause(where, where_criterias);
		query += where;
	}

	std::unique_ptr<mysql::result> p_result;
	mp_impl->p_connection->query(p_result, query);

	return p_result->updated_rows() > 0;
}

bool model::insert(std::string table_name, row_list_type rows)
{
	return false;
}

bool model::replace(std::string table_name, row_type row)
{
	std::string query {(boost::format("replace `%s`") % table_name).str()};

	{
		std::string field_list;
		build_field_clause(field_list, row);
		query += "(" + field_list + ")";
	}

	{
		std::string value_list;
		build_value_clause(value_list, row);
		query += value_list;
	}

	std::unique_ptr<mysql::result> p_result;
	mp_impl->p_connection->query(p_result, query);

	return p_result->updated_rows() > 0;
}

} // webpp
