#ifndef HPP_BUILDER_QUERY_WEBPP
#	define HPP_BUILDER_QUERY_WEBPP

#	include "memory.hpp"
#	include "query_clause.hpp"
#	include "query_var.hpp"
#	include "query_schema.hpp"

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
		builder select(std::vector<schema::field> field_list) const;
		builder insert(schema::table table_name) const;
		builder insert(schema::table table_name, std::vector<schema::field> field_list) const;
		builder replace(schema::table table_name) const;
		builder replace(schema::table table_name, std::vector<schema::field> field_list) const;
		builder update(schema::table table_name) const;

		// Continuation clauses
		builder from(schema::table table_name) const;
		builder where(schema::field field, std::shared_ptr<value> p_field_value) const;
		builder and_(schema::field field, std::shared_ptr<value> p_field_value) const;
		builder values(std::vector<std::vector<std::shared_ptr<value>>> tuple_list) const;
		builder set(std::map<schema::field, std::shared_ptr<value>> value_pair_list) const;

		builder & from(schema::table table_name);
		builder & where(schema::field field, std::shared_ptr<value> p_value);
		builder & and_(schema::field field, std::shared_ptr<value> p_value);
		builder & values(std::vector<std::vector<std::shared_ptr<value>>> tuple_list);
		builder & set(std::map<schema::field, std::shared_ptr<value>> value_pair_list);

		operator query() const;

	protected:
		template <typename clause_type>
		void verify_clause_is_allowed(clause_type & clause);
		template <typename clause_type>
		void push_clause(std::shared_ptr<clause_type> const & p_clause);

		builder fields(std::vector<schema::field> field_list, bool parenthesis = false) const;

		builder & fields(std::vector<schema::field> field_list, bool parenthesis = false);
		builder & select(std::vector<schema::field> field_list);
		builder & replace(schema::table table_name);
		builder & replace(schema::table table_name, std::vector<schema::field> field_list);
		builder & insert(schema::table table_name);
		builder & insert(schema::table table_name, std::vector<schema::field> field_list);
		builder & update(schema::table table_name);
};

class simple_builder
	: public builder
{
	public:
		explicit simple_builder(escaper const & e);
		simple_builder(builder const & copied);
		simple_builder(simple_builder const & copied);
		~simple_builder();

		// Initiator clauses
		simple_builder select() const;
		simple_builder select(std::vector<std::string> field_list) const;
		simple_builder insert(std::string table_name) const;
		simple_builder insert(std::string table_name, std::vector<std::string> field_list) const;
		simple_builder replace(std::string table_name) const;
		simple_builder replace(std::string table_name, std::vector<std::string> field_list) const;
		simple_builder update(std::string table_name) const;

		// Continuation clauses
		simple_builder from(std::string table_name) const;
		simple_builder where(std::string field_name, std::string field_value) const;
		simple_builder and_(std::string field_name, std::string field_value) const;
		simple_builder values(std::vector<std::vector<std::string>> tuple_list) const;
		simple_builder set(std::map<std::string, std::string> field_list) const;

		simple_builder & from(std::string table_name);
		simple_builder & where(std::string field_name, std::string field_value);
		simple_builder & and_(std::string field_name, std::string field_value);
		simple_builder & values(std::vector<std::vector<std::string>> tuple_list);
		simple_builder & set(std::map<std::string, std::string> field_list);

		using builder::operator query;

	private:
		simple_builder fields(std::vector<std::string> field_list, bool parenthesis = false) const;

		simple_builder & fields(std::vector<std::string> field_list, bool parenthesis = false);
		simple_builder & select(std::vector<std::string> field_list);
		simple_builder & replace(std::string table_name);
		simple_builder & replace(std::string table_name, std::vector<std::string> field_list);
		simple_builder & insert(std::string table_name);
		simple_builder & insert(std::string table_name, std::vector<std::string> field_list);
		simple_builder & update(std::string table_name);

		using builder::push_clause;
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
