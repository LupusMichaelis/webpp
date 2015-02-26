
#include "json.hpp"
#include "query_var.hpp"
#include "mysql_result.hpp"
#include "convert_rows_to_json.hpp"

namespace webpp {

convert_mysql_to_json::convert_mysql_to_json()
	: mp_yield()
{
}

void convert_mysql_to_json::convert(std::unique_ptr<json::value> & p_jv, query::var & v)
{
	v.accept(*this);
	std::swap(mp_yield, p_jv);
}

void convert_mysql_to_json::visit(mysql::boolean & v)
{
	if(nullptr == v)
		yield_null();
	else
		json::build<json::boolean, bool>(mp_yield, bool(v));
}

void convert_mysql_to_json::visit(mysql::integer & v)
{
	if(nullptr == v)
		yield_null();
	else
		json::build<json::number, long long>(mp_yield, v.get());
}

void convert_mysql_to_json::visit(mysql::string & v)
{
	if(nullptr == v)
		yield_null();
	else
		json::build<json::string, std::string>(mp_yield, v.get());
}

void convert_mysql_to_json::yield_null()
{
	mp_yield = std::move(std::make_unique<json::null>());
}

convert_mysql_to_json::~convert_mysql_to_json()
{
}

} // namespace webpp

webpp::json::array & operator << (webpp::json::array & array, webpp::model::row_list_type & rows)
{
	webpp::convert_mysql_to_json converter;

	for(auto & row: rows)
	{
		std::unique_ptr<webpp::json::object> p_object;
		webpp::json::build(p_object);

		for(auto & field: row)
		{
			std::unique_ptr<webpp::json::value> p_jv;

			converter.convert(p_jv, *field.second);
			webpp::json::add_property(*p_object, field.first, p_jv);
		}

		webpp::json::add(array, p_object);
	}

	return array;
}
