#ifndef HPP_CLAUSE_CLAUSE_QUERY_WEBPP
#	define HPP_CLAUSE_CLAUSE_QUERY_WEBPP

#	include "memory.hpp"

#	include <map>
#	include <string>
#	include <vector>

namespace webpp {
namespace query {

namespace clause {

class base;

class select;
class insert;

class fields;

class from;
class where;
class and_;

class values;

class visitor
{
	public:
		virtual void visit(select & clause) = 0;
		virtual void visit(insert & clause) = 0;
		virtual void visit(fields & clause) = 0;
		virtual void visit(from & clause) = 0;
		virtual void visit(where & clause) = 0;
		virtual void visit(and_ & clause) = 0;
		virtual void visit(values & clause) = 0;

		virtual ~visitor() { };
};

template <typename former_type, typename latter_type>
struct follows
{
	static bool const value = false;
};

class base
{
	public:
		base();

		virtual void clone(std::unique_ptr<base> & p_cloned) = 0;
		virtual void accept(visitor & visitor) = 0;
		virtual ~base();
};

class select : public base
{
	public:
		explicit select();

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & visitor);
		virtual ~select();
};

class fields : public base
{
	std::vector<std::string> m_field_list;
	bool m_parenthesis = false;

	public:
		fields(std::vector<std::string> const field_list, bool parenthesis);

		std::vector<std::string> const & field_list() const;
		bool const parenthesis() const;

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~fields();
};

class insert : public base
{
	std::string m_table_name;
	std::vector<std::string> m_field_list;

	public:
		explicit insert(std::string table_name, std::vector<std::string> const field_list);

		std::string const & table_name() const;
		std::vector<std::string> const & field_list() const;

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~insert();
};

class values : public base
{
	std::vector<std::vector<std::string>> m_value_list;

	public:
		explicit values(std::vector<std::vector<std::string>> const value_list);

		std::vector<std::vector<std::string>> const & value_list() const;

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~values();
};

class from : public base
{
	std::string m_table_name;

	public:
		explicit from(std::string table_name);

		std::string const & table_name();

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~from();
};

class where : public base
{
	std::string m_lhs, m_rhs;

	public:
		where(std::string lhs, std::string rhs);

		std::string const & lhs();
		std::string const & rhs();

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~where();
};

class and_ : public base
{
	std::string m_lhs, m_rhs;

	public:
		and_(std::string lhs, std::string rhs);

		std::string const & lhs();
		std::string const & rhs();

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~and_();
};

template<> struct follows<select, fields> { static bool const value = true; };
template<> struct follows<fields, from> { static bool const value = true; };
template<> struct follows<from, where> { static bool const value = true; };
template<> struct follows<where, and_> { static bool const value = true; };
template<> struct follows<and_, and_> { static bool const value = true; };

template<> struct follows<insert, fields> { static bool const value = true; };
template<> struct follows<fields, values> { static bool const value = true; };

template<typename next_clause_type>
class checker
	: public visitor
{
	next_clause_type & m_former;
	bool m_is_valid;

	template<typename previous_clause_type>
	void visit(previous_clause_type & latter)
	{
		m_is_valid = follows<previous_clause_type, next_clause_type>::value;
	}

	public:
		checker(base & lhs)
			: m_former(dynamic_cast<next_clause_type &>(lhs))
			, m_is_valid(false)
		{ }

		virtual void check(base & clause)
		{
			clause.accept(*this);
		}

		virtual void visit(select & clause)
		{
			visit<>(clause);
		}

		virtual void visit(insert & clause)
		{
			visit<>(clause);
		}

		virtual void visit(fields & clause)
		{
			visit<>(clause);
		}

		virtual void visit(from & clause)
		{
			visit<>(clause);
		}

		virtual void visit(where & clause)
		{
			visit<>(clause);
		}

		virtual void visit(and_ & clause)
		{
			visit<>(clause);
		}

		virtual void visit(values & clause)
		{
			visit<>(clause);
		}

		operator bool() { return m_is_valid; }
};

} // namespace clause

}} // namespace webpp::query

#endif // HPP_CLAUSE_QUERY_WEBPP
