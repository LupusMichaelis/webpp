#ifndef HPP_MODEL_WEBPP
#	define HPP_MODEL_WEBPP

#	include "memory.hpp"
#	include "query_var.hpp"

#	include <map>
#	include <string>
#	include <vector>

namespace webpp {

class story;

namespace mysql {
class connection;
class result;
class var;
}

namespace query {
class simple_builder;

namespace schema {
class table;
class field;
}
}

class model
{
	struct impl;
	std::unique_ptr<impl> mp_impl;

	query::simple_builder const & query_builder() const;

	public:
		typedef std::vector<std::shared_ptr<query::var>>
													row_type;
		typedef std::vector<row_type>				row_list_type;
		typedef std::vector<std::string>			field_list_type;

		typedef std::pair<query::schema::field, std::shared_ptr<query::var>>
													criteria_type;
		typedef std::vector<criteria_type>			criteria_list_type;

	public:
		model();
		~model();

		void connection(std::shared_ptr<mysql::connection> p_connection);

		void get_by_criterias
			( std::unique_ptr<field_list_type> & p_fields
			, std::unique_ptr<row_list_type> & p_rows
			, std::string table_name
			, criteria_list_type criterias
			);
		bool update(std::string table_name
				, criteria_list_type set_criterias
				, criteria_list_type where_criterias
				);
		bool insert(std::string table_name
				, row_list_type rows
				);
		bool replace(std::string table_name
				, row_type row
				);
};

} // webpp

#endif // HPP_MODEL_WEBPP
