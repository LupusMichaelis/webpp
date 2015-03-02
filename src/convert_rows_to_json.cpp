
#include "json.hpp"
#include "query_var.hpp"
#include "query_schema.hpp"
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

void convert_mysql_to_json::visit(query::boolean & v)
{
	if(nullptr == v)
		yield_null();
	else
		json::build<json::boolean, bool>(mp_yield, bool(v));
}

void convert_mysql_to_json::visit(query::integer & v)
{
	if(nullptr == v)
		yield_null();
	else
		json::build<json::number, long long>(mp_yield, v.get());
}

void convert_mysql_to_json::visit(query::string & v)
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

json::array & cast(json::array & array
		, model::field_list_type & fields
		, model::row_list_type & rows
		)
{
	convert_mysql_to_json converter;

	for(auto & row: rows)
	{
		std::unique_ptr<json::object> p_object;
		json::build(p_object);

		for(size_t idx = 0; idx < fields.size(); ++idx)
		{
			auto p_value = row[idx];
			auto field = fields[idx];

			std::unique_ptr<json::value> p_jv;

			converter.convert(p_jv, *p_value);
			json::add_property(*p_object, field.name(), p_jv);
		}

		json::add(array, p_object);
	}

	return array;
}

} // namespace webpp

