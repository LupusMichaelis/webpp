
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

#include <cstdlib>

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

	std::vector<std::pair<std::string, enum enum_field_types>> field_list;
	while(MYSQL_FIELD * field = mysql_fetch_field(result))
		field_list.push_back({field->name, field->type});

	while(MYSQL_ROW row = mysql_fetch_row(result))
	{
		row_type row_;

		for(size_t row_idx = 0; row_idx < field_list.size(); ++row_idx)
		{
			std::string const & field_name {field_list[row_idx].first};
			enum enum_field_types const & field_type {field_list[row_idx].second};

			if(NULL == row[row_idx])
				row_[field_name] = nullptr;
			else
				switch(field_type)
				{
					case MYSQL_TYPE_DECIMAL:
					case MYSQL_TYPE_TINY:
					case MYSQL_TYPE_SHORT:
					case MYSQL_TYPE_LONG:
					case MYSQL_TYPE_LONGLONG:
					case MYSQL_TYPE_INT24:
						row_[field_name] = std::atoi(row[row_idx]);
						break;
					case MYSQL_TYPE_NULL:
						row_[field_name] = nullptr;
						break;

					// MYSQL_TYPE_FLOAT
					// MYSQL_TYPE_DOUBLE
					// MYSQL_TYPE_TIMESTAMP
					// MYSQL_TYPE_DATE
					// MYSQL_TYPE_TIME
					// MYSQL_TYPE_DATETIME
					// MYSQL_TYPE_YEAR
					// MYSQL_TYPE_NEWDATE
					// MYSQL_TYPE_VARCHAR
					// MYSQL_TYPE_BIT
					default:
						row_[field_name] = std::string(row[row_idx]);
				}
		}

		rows.push_back(row_);
	}

	mysql_free_result(result);

}
