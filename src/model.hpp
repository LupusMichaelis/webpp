#ifndef HPP_MODEL_WEBPP
#	define HPP_MODEL_WEBPP

#	include "memory.hpp"
#	include "mysql_var.hpp"

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

class model
{
	struct impl;
	std::unique_ptr<impl> mp_impl;

	public:
		typedef std::map<std::string, std::shared_ptr<mysql::var>>
													row_type;
		typedef std::vector<row_type>				row_list_type;

	public:
		model();
		~model();

		void connection(std::shared_ptr<mysql::connection> p_connection);

		void get_by_criterias(std::unique_ptr<row_list_type> & rows
			, std::string table_name
			, row_type criterias
			);
		bool update(std::string table_name
				, row_type set_criterias
				, row_type where_criterias
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
