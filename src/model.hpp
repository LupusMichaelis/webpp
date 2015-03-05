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
class value;
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
		typedef std::vector<std::shared_ptr<query::value>>
													row_type;
		typedef std::vector<row_type>				row_list_type;
		typedef std::vector<query::schema::field>
													field_list_type;

		typedef std::map<query::schema::field, std::shared_ptr<query::value>>
													criteria_list_type;
		typedef typename criteria_list_type::value_type
													criteria_type;

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
				, field_list_type const & fields
				, row_list_type const & original_rows
				, row_list_type const & updated_rows
				, criteria_list_type where_criterias
				);
		bool insert(std::string table_name
				, row_list_type rows
				);
};

} // webpp

#endif // HPP_MODEL_WEBPP
