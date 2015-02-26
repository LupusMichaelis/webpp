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

class update;
class set;

class replace;

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
		virtual void visit(update & clause) = 0;
		virtual void visit(set & clause) = 0;
		virtual void visit(replace & clause) = 0;

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

class replace : public base
{
	std::string m_table_name;
	std::vector<std::string> m_field_list;

	public:
		explicit replace(std::string table_name, std::vector<std::string> const field_list);

		std::string const & table_name() const;
		std::vector<std::string> const & field_list() const;

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~replace();
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

		std::string const & table_name() const;

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~from();
};

class where : public base
{
	std::string m_lhs, m_rhs;

	public:
		where(std::string lhs, std::string rhs);

		std::string const & lhs() const;
		std::string const & rhs() const;

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~where();
};

class and_ : public base
{
	std::string m_lhs, m_rhs;

	public:
		and_(std::string lhs, std::string rhs);

		std::string const & lhs() const;
		std::string const & rhs() const;

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~and_();
};

class update : public base
{
	std::string m_table;

	public:
		explicit update(std::string table);

		std::string const & table_name() const;

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~update();
};

class set : public base
{
	std::map<std::string, std::string> m_field_list;

	public:
		set(std::map<std::string, std::string> field_list);
		set(std::string field_name, std::string value_name);

		std::map<std::string, std::string> const & field_list();

		virtual void clone(std::unique_ptr<base> & p_cloned);
		virtual void accept(visitor & v);
		virtual ~set();
};

template<> struct follows<select, fields> { static bool const value = true; };
template<> struct follows<fields, from> { static bool const value = true; };
template<> struct follows<from, where> { static bool const value = true; };
template<> struct follows<where, and_> { static bool const value = true; };
template<> struct follows<and_, and_> { static bool const value = true; };

template<> struct follows<replace, fields> { static bool const value = true; };
template<> struct follows<insert, fields> { static bool const value = true; };
template<> struct follows<fields, values> { static bool const value = true; };

template<> struct follows<update, set> { static bool const value = true; };
template<> struct follows<set, where> { static bool const value = true; };

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

		virtual void check(base & clause)		{ clause.accept(*this); }
		virtual void visit(select & clause)		{ visit<>(clause); }
		virtual void visit(insert & clause)		{ visit<>(clause); }
		virtual void visit(fields & clause)		{ visit<>(clause); }
		virtual void visit(from & clause)		{ visit<>(clause); }
		virtual void visit(where & clause)		{ visit<>(clause); }
		virtual void visit(and_ & clause)		{ visit<>(clause); }
		virtual void visit(values & clause)		{ visit<>(clause); }
		virtual void visit(update & clause)		{ visit<>(clause); }
		virtual void visit(set & clause)		{ visit<>(clause); }
		virtual void visit(replace & clause)	{ visit<>(clause); }

		operator bool() { return m_is_valid; }
};

} // namespace clause

}} // namespace webpp::query

#endif // HPP_CLAUSE_QUERY_WEBPP
