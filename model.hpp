#ifndef HPP_MODEL
#	define HPP_MODEL

#	include "memory.hpp"
#	include "mysql_var.hpp"

#	include <map>
#	include <string>
#	include <vector>

class story;

class model
{
	class impl;
	std::unique_ptr<impl> mp_impl;

	public:
		typedef std::map<std::string, std::string> criteria_type;
		typedef std::map<std::string, mysql_var> row_type;
		typedef std::vector<row_type> row_list_type;

	public:
		model();
		~model();

		void get_rows_by_criterias(row_list_type & rows, std::string const table_name, criteria_type const criterias);
};

#endif // HPP_MODEL
