#ifndef HPP_VAR_QUERY_WEBPP
#	define HPP_VAR_QUERY_WEBPP

#	include "memory.hpp"
#	include <string>
/**
 * \todo boolean
 * \todo the operator == algorithm is flawed, need to review the logic
 */

namespace webpp { namespace query {

class boolean;
class integer;
class string;

class visitor
{
	public:
		virtual void visit(boolean & v) = 0;
		virtual void visit(integer & v) = 0;
		virtual void visit(string & v) = 0;
		virtual ~visitor();
};

class const_visitor
{
	public:
		virtual void visit(boolean const & v) = 0;
		virtual void visit(integer const & v) = 0;
		virtual void visit(string const & v) = 0;
		virtual ~const_visitor();
};

class var
{
	struct impl;
	std::unique_ptr<impl> mp_impl;

	protected:
		void set_not_null();

	public:

		var();
		virtual ~var();

		var(var const & copied);
		explicit var(std::nullptr_t);
		virtual var & operator=(var const & copied);
		virtual operator bool() const;

		virtual var & operator=(std::nullptr_t);

		bool operator ==(std::nullptr_t) const;

		virtual void accept(const_visitor & v) const = 0;
		virtual void accept(visitor & v) = 0;
		virtual void clone(std::shared_ptr<var> & p_v) const = 0;
};

class string
	: public var
{
	std::string m_value;

	public:
		string();
		string(string const & copied);
		explicit string(std::string const & copied);
		virtual string & operator=(string const & copied);

		virtual operator bool() const;

		virtual std::string const & get() const;
		virtual void set(std::string const & new_value);

		using var::operator ==;
		bool operator ==(string const & rhs) const;
		bool operator ==(std::string const & rhs) const;

		virtual void accept(visitor & v);
		virtual void accept(const_visitor & v) const;
		virtual void clone(std::shared_ptr<var> & p_v) const;
		virtual ~string();
};

class integer
	: public var
{
	long long m_value;

	public:
		integer();
		integer(integer const & copied);
		explicit integer(std::string const & copied);
		explicit integer(long long const copied);
		virtual integer & operator=(integer const & copied);

		virtual operator bool() const;

		virtual long long const & get() const;
		virtual void set(long long const new_value);

		using var::operator ==;
		bool operator ==(integer const & rhs) const;
		bool operator ==(long long const rhs) const;

		virtual void accept(visitor & v);
		virtual void accept(const_visitor & v) const;
		virtual void clone(std::shared_ptr<var> & p_v) const;
		virtual ~integer();
};

class boolean
	: public var
{
	bool m_value;

	public:
		boolean();
		boolean(boolean const & copied);
		explicit boolean(std::string const & copied);
		explicit boolean(bool const copied);
		virtual boolean & operator=(boolean const & copied);
		virtual boolean & operator=(bool const copied);

		virtual operator bool() const;

		using var::operator ==;
		bool operator ==(boolean const & rhs) const;
		bool operator ==(bool const rhs) const;

		virtual void accept(visitor & v);
		virtual void accept(const_visitor & v) const;
		virtual void clone(std::shared_ptr<var> & p_v) const;
		virtual ~boolean();
};

template <typename value_type>
void build(std::shared_ptr<var> & p_node)
{
	p_node = std::move(std::make_shared<value_type>());
}

} } // namespace webpp::query

inline bool operator !=(webpp::query::var const & lhs, std::nullptr_t)
{
	return !(lhs == nullptr);
}

inline bool operator !=(std::nullptr_t, webpp::query::var const & rhs)
{
	return rhs != nullptr;
}

inline bool operator ==(std::nullptr_t, webpp::query::var const & rhs)
{
	return rhs == nullptr;
}

bool operator ==(std::string const & lhs, webpp::query::var const & rhs);

#endif // HPP_VAR_QUERY_WEBPP
