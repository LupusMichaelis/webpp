
#include "mysql_connection.hpp"
#include "mysql_result.hpp"

#include <mysql.h>

namespace webpp { namespace mysql {

struct connection::impl
{
	MYSQL * p_mysql;
};

connection::connection()
	: mp_impl {std::make_unique<impl>()}
{
	mp_impl->p_mysql = mysql_init(NULL);
}

connection::~connection()
{
	mysql_close(mp_impl->p_mysql);
}

connection::operator bool() const
{
	return mp_impl->p_mysql;
}

void connection::connect(std::string const & host
		, std::string username
		, std::string password
		, std::string space
		, unsigned port
		)
{
	mysql_real_connect
		( mp_impl->p_mysql
		, host.c_str()
		, username.c_str()
		, password.c_str()
		, space.c_str()
		, port
		, NULL // unix_socket->c_str()
		, 0 // XXX client_flag!!!
		);
}

void connection::query(std::unique_ptr<result> & p_result, std::string const & query)
{
	if(mysql_query(mp_impl->p_mysql, query.c_str()))
		throw mysql_error(mp_impl->p_mysql);

	MYSQL_RES * p_native_result = NULL;
	p_native_result = mysql_store_result(mp_impl->p_mysql);
	if(0 == mysql_num_rows(p_native_result))
		throw "Too much or too many results";

	result::field_list_type field_list;
	while(MYSQL_FIELD * field = mysql_fetch_field(p_native_result))
		field_list.push_back({field->name, field->type});

	result::row_list_type rows;

	while(MYSQL_ROW row = mysql_fetch_row(p_native_result))
	{
		result::row_type row_;

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

	mysql_free_result(p_native_result);

	auto tp_result = std::make_unique<result>();
	tp_result->fields(field_list);
	tp_result->rows(rows);

	std::swap(tp_result, p_result);
}

} } // namespace webpp::mysql
