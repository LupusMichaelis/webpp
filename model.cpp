
#include "model.hpp"

#include <mysql.h>
#include <boost/format.hpp>

struct model::impl
{
	MYSQL * p_mysql;
};

model::model()
	: mp_impl {std::make_unique<model::impl>()}
{
	mp_impl->p_mysql = mysql_init(NULL);
	mysql_real_connect(mp_impl->p_mysql, "localhost", "test", "test", "test", 3308, NULL, 0);
}

model::~model()
{
	mysql_close(mp_impl->p_mysql);
}

void model::get_rows_by_criterias(row_list_type &rows, std::string const table_name, criteria_type const criterias)
{
	auto it_criteria = criterias.cbegin();
	auto query = boost::format("select * from `%s` where `%s` = '%s'")
		% table_name % it_criteria->first % it_criteria->second;

	if(mysql_query(mp_impl->p_mysql, query.str().c_str()))
		throw mysql_error(mp_impl->p_mysql);

	MYSQL_RES * result = NULL;
	result = mysql_store_result(mp_impl->p_mysql);
	if(0 == mysql_num_rows(result))
		throw "Too much or too many results";

	std::vector<std::string> field_list;
	while(MYSQL_FIELD * field = mysql_fetch_field(result))
		field_list.push_back(field->name);

	while(MYSQL_ROW row = mysql_fetch_row(result))
	{
		row_type row_;

		row_[field_list[0]] = row[0] ? row[0]: "";
		row_[field_list[1]] = row[1] ? row[1]: "";
		row_[field_list[2]] = row[2] ? row[2]: "";
		row_[field_list[3]] = row[3] ? row[3]: "";

		rows.push_back(row_);
	}

	mysql_free_result(result);

}
