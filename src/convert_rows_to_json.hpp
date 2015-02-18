#ifndef  HPP_JSON_MY_WEBPP
#	define  HPP_JSON_MY_WEBPP

#	include "mysql_var.hpp"

namespace webpp
{

namespace json {

class value;
class array;

}

class convert_mysql_to_json
	: public mysql::visitor
{
	std::unique_ptr<json::value> mp_yield;

	public:
		convert_mysql_to_json();

		void convert(std::unique_ptr<json::value> & p_jv, mysql::var & v);
		void yield_null();

		virtual void visit(mysql::boolean & v);
		virtual void visit(mysql::integer & v);
		virtual void visit(mysql::string & v);
		virtual ~convert_mysql_to_json();
};

}; // namespace webpp

#	include "model.hpp"

webpp::json::array & operator << (webpp::json::array & array, webpp::model::row_list_type & result);

#endif //  HPP_JSON_MY_WEBPP
