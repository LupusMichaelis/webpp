#ifndef HPP_RESULT_MYSQL_WEBPP
#	define HPP_RESULT_MYSQL_WEBPP

#	include "memory.hpp"
#	include "query_var.hpp"

#	include <vector>
#	include <map>
#	include <string>
#	include <mysql.h>

namespace webpp { namespace query { namespace schema {
class field;
} } } // namespace webpp::query::schema

namespace webpp { namespace mysql {

class result
{
	struct impl;
	std::unique_ptr<impl> mp_impl;

	public:
		typedef std::vector<std::shared_ptr<query::schema::field>>
													field_list_type;

		typedef std::vector<std::shared_ptr<query::var>>
													row_type;
		typedef std::vector<row_type>				row_list_type;

		typedef std::map<query::schema::field, std::shared_ptr<query::var>>
													criteria_type;
		typedef std::vector<criteria_type>			criteria_list_type;

		result();
		~result();

		operator bool() const;

		void fields(field_list_type field_list);
		void fields(std::unique_ptr<field_list_type> & p_field_list) const;

		void rows(row_list_type row_list);
		void rows(std::unique_ptr<row_list_type> & p_row_list) const;

		void updated_rows(size_t update_rows);
		size_t updated_rows() const;

};

} } // namespace webpp::mysql

#endif // HPP_RESULT_MYSQL_WEBPP

