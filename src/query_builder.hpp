#ifndef HPP_BUILDER_QUERY_WEBPP
#	define HPP_BUILDER_QUERY_WEBPP

#	include "memory.hpp"
#	include "query_clause.hpp"

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
		explicit builder(escaper const & e);
		builder(builder const & copied);
		~builder();

		// Initiator clauses
		builder select() const;
		builder select(std::vector<std::string> field_list) const;
		builder insert(std::string table_name, std::vector<std::string> field_list) const;
		builder replace(std::string table_name, std::vector<std::string> field_list) const;
		builder update(std::string table_name) const;

		// Continuation clauses
		builder from(std::string table_name) const;
		builder where(std::string field_name, std::string field_value) const;
		builder and_(std::string field_name, std::string field_value) const;
		builder values(std::vector<std::vector<std::string>> field_list) const;
		builder set(std::map<std::string, std::string> field_list) const;

		builder & from(std::string table_name);
		builder & where(std::string field_name, std::string field_value);
		builder & and_(std::string field_name, std::string field_value);
		builder & values(std::vector<std::vector<std::string>> field_list);
		builder & set(std::map<std::string, std::string> field_list);

		operator query() const;

	private:
		builder fields(std::vector<std::string> field_list, bool parenthesis = false) const;

		builder & fields(std::vector<std::string> field_list, bool parenthesis = false);
		builder & select(std::vector<std::string> field_list);
		builder & insert(std::string table_name, std::vector<std::string> field_list);
		builder & replace(std::string table_name, std::vector<std::string> field_list);
		builder & update(std::string table_name);

		template <typename clause_type>
		void verify_clause_is_allowed(clause_type & clause);
		template <typename clause_type>
		void push_clause(std::shared_ptr<clause_type> const & p_clause);
};

extern template
void builder::verify_clause_is_allowed<clause::select>(clause::select & clause);
extern template
void builder::push_clause<clause::select>(std::shared_ptr<clause::select> const & p_clause);

extern template
void builder::verify_clause_is_allowed<clause::insert>(clause::insert & clause);
extern template
void builder::push_clause<clause::insert>(std::shared_ptr<clause::insert> const & p_clause);

extern template
void builder::verify_clause_is_allowed<clause::from>(clause::from & clause);
extern template
void builder::push_clause<clause::from>(std::shared_ptr<clause::from> const & p_clause);

extern template
void builder::verify_clause_is_allowed<clause::where>(clause::where & clause);
extern template
void builder::push_clause<clause::where>(std::shared_ptr<clause::where> const & p_clause);

extern template
void builder::verify_clause_is_allowed<clause::and_>(clause::and_ & clause);
extern template
void builder::push_clause<clause::and_>(std::shared_ptr<clause::and_> const & p_clause);

extern template
void builder::verify_clause_is_allowed<clause::values>(clause::values & clause);
extern template
void builder::push_clause<clause::values>(std::shared_ptr<clause::values> const & p_clause);

extern template
void builder::verify_clause_is_allowed<clause::set>(clause::set & clause);
extern template
void builder::push_clause<clause::set>(std::shared_ptr<clause::set> const & p_clause);

extern template
void builder::verify_clause_is_allowed<clause::replace>(clause::replace & clause);
extern template
void builder::push_clause<clause::replace>(std::shared_ptr<clause::replace> const & p_clause);

}} // namespace webpp::query

#endif // HPP_BUILDER_QUERY_WEBPP
