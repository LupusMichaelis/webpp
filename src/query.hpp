#ifndef HPP_QUERY_WEBPP
#	define HPP_QUERY_WEBPP

#	include "memory.hpp"

#	include <map>
#	include <string>
#	include <vector>

namespace webpp {
namespace query {

class query;
class escaper;

class builder
{
	struct impl;
	std::unique_ptr<impl> mp_impl;

	public:
		builder();
		~builder();

		query select(std::vector<std::string> fields) const;
};

class query
{
	struct impl;
	std::unique_ptr<impl> mp_impl;

	public:
		explicit query(escaper const & e);
		query(query const & rhs);
		~query();

		query select(std::vector<std::string> field_list) const;
		query from(std::string table_name) const;
		query where(std::string field_name, std::string field_value) const;
		query and_(std::string field_name, std::string field_value) const;

		std::string const str() const;
};

class escaper
{
};

}} // namespace webpp::query

#endif // HPP_QUERY_WEBPP
