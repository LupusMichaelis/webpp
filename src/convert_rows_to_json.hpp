#ifndef  HPP_JSON_MY_WEBPP
#	define  HPP_JSON_MY_WEBPP

#	include "model.hpp"

namespace webpp
{

namespace query
{
	class var;
}

namespace json {

class value;
class array;

}

class convert_mysql_to_json
	: public query::visitor
{
	std::unique_ptr<json::value> mp_yield;

	public:
		convert_mysql_to_json();

		void convert(std::unique_ptr<json::value> & p_jv, query::var & v);
		void yield_null();

		virtual void visit(query::boolean & v);
		virtual void visit(query::integer & v);
		virtual void visit(query::string & v);
		virtual ~convert_mysql_to_json();
};

json::array & cast(webpp::json::array & array
		, webpp::model::field_list_type & fields
		, webpp::model::row_list_type & rows
		);

}; // namespace webpp


#endif //  HPP_JSON_MY_WEBPP
