#ifndef HPP_QUERY_WEBPP
#	define HPP_QUERY_WEBPP

#	include "memory.hpp"
#	include "query_clause.hpp"

#	include <map>
#	include <string>
#	include <vector>

namespace webpp {
namespace query {

class escaper
{
	public:
		escaper() {};
};

class query
	: public clause::visitor
{
	struct impl;
	std::unique_ptr<impl> mp_impl;

	public:
		query(escaper const & e);
		query(query const & rhs);
		~query();

		std::string const str() const;
		void operator() (std::shared_ptr<clause::base> const & p_clause);
		void operator() (clause::base & clause);

		void visit(clause::fields & clause);

		void visit(clause::select & clause);
		void visit(clause::from & clause);
		void visit(clause::where & clause);
		void visit(clause::and_ & clause);

		void visit(clause::insert & clause);
		void visit(clause::values & clause);

		void visit(clause::update & clause);
		void visit(clause::set & clause);

		void visit(clause::replace & clause);
};

}} // namespace webpp::query

#endif // HPP_QUERY_WEBPP
