
#include "mysql_connection.hpp"
#include "mysql_result.hpp"
#include "query_var.hpp"

#include "query.hpp"

#include <mysql.h>
#include <cstdlib>

namespace webpp { namespace mysql {

class converter
	: public query::visitor
{
	std::string m_literal;
	public:
		converter(std::string const & literal)
			: m_literal {literal} { }

		virtual void visit(query::boolean & v)
		{
			throw "Not supported";
		}

		virtual void visit(query::integer & v)
		{
			v.set(std::atoi(m_literal.c_str()));
		}

		virtual void visit(query::string & v)
		{
			v.set(m_literal);
		}

		virtual ~converter()
		{
		}
};

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

void connection::connect(std::string const host
		, std::string const username
		, std::string const password
		, std::string const space
		, unsigned const port
		)
{
	MYSQL * p = mysql_real_connect
		( mp_impl->p_mysql
		, host.c_str()
		, username.c_str()
		, password.c_str()
		, space.c_str()
		, port
		, NULL // unix_socket->c_str()
		, 0 // XXX client_flag!!!
		);

	if(NULL == p)
		throw mysql_error(mp_impl->p_mysql);
}

void connection::query(std::unique_ptr<result> & p_result, webpp::query::query const & q)
{
	std::string query = q.str();

	if(mysql_query(mp_impl->p_mysql, query.c_str()))
		throw mysql_error(mp_impl->p_mysql);

	MYSQL_RES * p_native_result = NULL;
	p_native_result = mysql_store_result(mp_impl->p_mysql);

	if(NULL == p_native_result && 0 != mysql_errno(mp_impl->p_mysql))
		throw mysql_error(mp_impl->p_mysql);

	auto tp_result = std::make_unique<result>();

	if(p_native_result)
	{
		if(0 == mysql_num_rows(p_native_result))
			throw "Too much or too many results";

		result::field_list_type field_list;
		while(MYSQL_FIELD * field = mysql_fetch_field(p_native_result))
		{
			auto p_field = std::make_shared<query::schema::field>(field->name);

			switch(field->type)
			{
				case MYSQL_TYPE_DECIMAL:
				case MYSQL_TYPE_TINY:
				case MYSQL_TYPE_SHORT:
				case MYSQL_TYPE_LONG:
				case MYSQL_TYPE_LONGLONG:
				case MYSQL_TYPE_INT24:
					p_field->of_type<query::integer>();
					break;
				case MYSQL_TYPE_NULL:
					p_field->of_type<query::string>();
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
					p_field->of_type<query::string>();
			}

			field_list.push_back(p_field);
		}

		result::row_list_type rows;

		while(MYSQL_ROW row = mysql_fetch_row(p_native_result))
		{
			result::row_type row_;

			for(size_t row_idx = 0; row_idx < field_list.size(); ++row_idx)
			{
				std::shared_ptr<query::var> p_value;
				field_list[row_idx]->clone(p_value);

				if(NULL != row[row_idx])
				{
					converter c {row[row_idx]};
					p_value->accept(c);
				}

				row_.push_back(std::move(p_value));
			}

			rows.push_back(row_);
		}

		tp_result->fields(field_list);
		tp_result->rows(rows);

		mysql_free_result(p_native_result);
	}

	size_t num_rows = mysql_affected_rows(mp_impl->p_mysql);
	tp_result->updated_rows(num_rows);

	std::swap(tp_result, p_result);
}

} } // namespace webpp::mysql
