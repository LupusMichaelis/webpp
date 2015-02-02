
#include "model.hpp"
#include "mysql_connection.hpp"
#include "mysql_result.hpp"

#include <cstdlib>

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

void model::connection(std::shared_ptr<mysql::connection> const & p_connection)
{
	mp_impl->p_connection = p_connection;
}
void model::get_rows_by_criterias
	( std::unique_ptr<row_list_type> & p_rows
	, std::string const table_name
	, criteria_type const criterias
	)
{
	std::string query {(boost::format("select * from `%s`") % table_name).str()};

	if(criterias.size())
	{
		std::vector<std::string> where;
		for(auto criteria: criterias)
			where.push_back((boost::format("`%s` = '%s'") % criteria.first % criteria.second).str());

		query += " where " + boost::algorithm::join(where, " and ");
	}

	std::unique_ptr<mysql::result> p_result;
	mp_impl->p_connection->query(p_result, query);
	p_result->rows(p_rows);
}

} // webpp
