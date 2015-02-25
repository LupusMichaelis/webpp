#ifndef HPP_QUERY_WEBPP
#	define HPP_QUERY_WEBPP

#	include "memory.hpp"

#	include <map>
#	include <string>
#	include <vector>

namespace webpp {
namespace query {

class escaper
{
};

namespace clause
{
	class base;

	class select;
	class insert;

	class fields;

	class from;
	class where;
	class and_;

	class values;
}

class query
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
};

class builder
{
	struct impl;
	std::unique_ptr<impl> mp_impl;

	public:
		explicit builder(escaper const & e);
		builder(builder const & copied);
		~builder();

		// Initiator clauses
		builder select(std::vector<std::string> field_list) const;
		builder insert(std::string table_name, std::vector<std::string> field_list) const;

		// Continuation clauses
		builder from(std::string table_name) const;
		builder where(std::string field_name, std::string field_value) const;
		builder and_(std::string field_name, std::string field_value) const;
		builder values(std::vector<std::vector<std::string>> field_list) const;

		builder & from(std::string table_name);
		builder & where(std::string field_name, std::string field_value);
		builder & and_(std::string field_name, std::string field_value);
		builder & values(std::vector<std::vector<std::string>> field_list);

		operator query() const;

	private:
		builder fields(std::vector<std::string> field_list, bool parenthesis = false) const;

		builder & fields(std::vector<std::string> field_list, bool parenthesis = false);
		builder & select(std::vector<std::string> field_list);
		builder & insert(std::string table_name, std::vector<std::string> field_list);
};

namespace clause {

class base
{
	public:
		base() {}
		virtual ~base() {}

		template <typename other_clause_type>
		void allow(other_clause_type & candidate)
		{
			throw "Not supported";
		}

		virtual void clone(std::unique_ptr<base> & p_cloned) = 0;
		virtual void verify(base & previous) = 0;
		virtual void accept(query & visitor) = 0;
};

class select : public base
{
	public:
		explicit select()
		{ }

		virtual ~select() {}

		virtual void clone(std::unique_ptr<base> & p_cloned)
		{
			std::unique_ptr<base> p_clonee = std::make_unique<select>(*this);
			std::swap(p_cloned, p_clonee);
		}

		virtual void verify(base & previous)
		{
			previous.allow(*this);
		}

		virtual void accept(query & visitor)
		{
			visitor.visit(*this);
		}
};

class fields : public base
{
	std::vector<std::string> m_field_list;
	bool m_parenthesis = false;
	public:
		fields(std::vector<std::string> const field_list, bool parenthesis)
			: m_field_list {field_list}
			, m_parenthesis(parenthesis)
		{ }

		virtual ~fields() {}

		virtual void clone(std::unique_ptr<base> & p_cloned)
		{
			std::unique_ptr<base> p_clonee = std::make_unique<fields>(*this);
			std::swap(p_cloned, p_clonee);
		}

		std::vector<std::string> const & field_list() const
		{
			return m_field_list;
		}

		bool const parenthesis() const
		{
			return m_parenthesis;
		}

		virtual void verify(base & previous)
		{
			previous.allow(*this);
		}

		virtual void accept(query & visitor)
		{
			visitor.visit(*this);
		}
};

class insert : public base
{
	std::string m_table_name;
	std::vector<std::string> m_field_list;

	public:
		explicit insert(std::string table_name, std::vector<std::string> const field_list)
			: m_table_name {table_name}
			, m_field_list {field_list}
		{ }

		virtual ~insert() {}

		virtual void clone(std::unique_ptr<base> & p_cloned)
		{
			std::unique_ptr<base> p_clonee = std::make_unique<insert>(*this);
			std::swap(p_cloned, p_clonee);
		}

		std::string const & table_name() const
		{
			return m_table_name;
		}

		std::vector<std::string> const & field_list() const
		{
			return m_field_list;
		}

		virtual void verify(base & previous)
		{
			previous.allow(*this);
		}

		virtual void accept(query & visitor)
		{
			visitor.visit(*this);
		}
};

class values : public base
{
	std::vector<std::vector<std::string>> m_value_list;

	public:
		explicit values(std::vector<std::vector<std::string>> const value_list)
			: m_value_list {value_list}
		{ }

		virtual ~values() {}

		virtual void clone(std::unique_ptr<base> & p_cloned)
		{
			std::unique_ptr<base> p_clonee = std::make_unique<values>(*this);
			std::swap(p_cloned, p_clonee);
		}

		std::vector<std::vector<std::string>> const & value_list() const
		{
			return m_value_list;
		}

		virtual void verify(base & previous)
		{
			previous.allow(*this);
		}

		virtual void accept(query & visitor)
		{
			visitor.visit(*this);
		}
};

class from : public base
{
	std::string m_table_name;

	public:
		explicit from(std::string table_name)
			: m_table_name {table_name}
		{ }

		virtual ~from() {}

		std::string const & table_name()
		{
			return m_table_name;
		}

		virtual void clone(std::unique_ptr<base> & p_cloned)
		{
			std::unique_ptr<base> p_clonee = std::make_unique<from>(*this);
			std::swap(p_cloned, p_clonee);
		}

		virtual void verify(base & previous)
		{
			previous.allow(*this);
		}

		virtual void accept(query & visitor)
		{
			visitor.visit(*this);
		}
};

class where : public base
{
	std::string m_lhs, m_rhs;

	public:
		where(std::string lhs, std::string rhs)
			: m_lhs {lhs}, m_rhs {rhs}
		{ }

		virtual ~where() {}

		std::string const & lhs()
		{
			return m_lhs;
		}

		std::string const & rhs()
		{
			return m_rhs;
		}

		virtual void clone(std::unique_ptr<base> & p_cloned)
		{
			std::unique_ptr<base> p_clonee = std::make_unique<where>(*this);
			std::swap(p_cloned, p_clonee);
		}

		virtual void verify(base & previous)
		{
			previous.allow(*this);
		}

		virtual void accept(query & visitor)
		{
			visitor.visit(*this);
		}
};

class and_ : public base
{
	std::string m_lhs, m_rhs;

	public:
		and_(std::string lhs, std::string rhs)
			: m_lhs {lhs}, m_rhs {rhs}
		{ }

		virtual ~and_() {}

		std::string const & lhs()
		{
			return m_lhs;
		}

		std::string const & rhs()
		{
			return m_rhs;
		}

		virtual void clone(std::unique_ptr<base> & p_cloned)
		{
			std::unique_ptr<base> p_clonee = std::make_unique<and_>(*this);
			std::swap(p_cloned, p_clonee);
		}

		virtual void verify(base & previous)
		{
			previous.allow(*this);
		}

		virtual void accept(query & visitor)
		{
			visitor.visit(*this);
		}
};

} // namespace clause

}} // namespace webpp::query

#endif // HPP_QUERY_WEBPP
