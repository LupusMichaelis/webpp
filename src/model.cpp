
#include "model.hpp"
#include "mysql_connection.hpp"
#include "mysql_result.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

namespace webpp {

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

void build_where_clause(std::string & where, model::criteria_type criterias)
{
	if(criterias.size())
	{
		std::vector<std::string> buffer;
		for(auto criteria: criterias)
			buffer.push_back((boost::format("`%s` = '%s'") % criteria.first % criteria.second).str());

		where = " where " + boost::algorithm::join(buffer, " and ");
	}
}

void build_set_clause(std::string & set, model::criteria_type criterias)
{
	if(criterias.size())
	{
		std::vector<std::string> buffer;
		for(auto criteria: criterias)
			buffer.push_back((boost::format("`%s` = '%s'") % criteria.first % criteria.second).str());

		set = " set " + boost::algorithm::join(buffer, ", ");
	}
}

void model::get_rows_by_criterias
	( std::unique_ptr<row_list_type> & p_rows
	, std::string table_name
	, criteria_type criterias
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

bool model::update_rows(std::string table_name
		, criteria_type set_criterias
		, criteria_type where_criterias
		)
{
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

} // webpp
