#ifndef HPP_MYSQL_VAR
#	define HPP_MYSQL_VAR

#	include "memory.hpp"
#	include <string>

namespace webpp { namespace mysql {

class boolean;
class integer;
class string;

class visitor
{
	public:
		/*
		virtual void visit(boolean & v) = 0;
		virtual void visit(integer & v) = 0;
		*/
		virtual void visit(string & v) = 0;
		virtual ~visitor();
};

class printer
{
	struct impl;
	std::unique_ptr<impl> mp_impl;

	public:
		explicit printer(std::ostream & out);
		virtual void visit(string & v);
		virtual ~printer();
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
		virtual bool operator ==(std::nullptr_t) const;

		virtual void accept(visitor & v) = 0;
};

class string : public var
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

		//using var::operator ==;
		virtual bool operator ==(std::nullptr_t) const
		{
			return var::operator ==(nullptr);
		}
		virtual bool operator ==(std::string const & rhs) const;

		virtual void accept(visitor & v);
		virtual ~string();
};

} } // namespace webpp::mysql

inline bool operator !=(webpp::mysql::var const & lhs, std::nullptr_t)
{
	return !(lhs == nullptr);
}

inline bool operator !=(std::nullptr_t, webpp::mysql::var const & rhs)
{
	return rhs != nullptr;
}

inline bool operator ==(std::nullptr_t, webpp::mysql::var const & rhs)
{
	return rhs == nullptr;
}

inline bool operator ==(std::string const & lhs, webpp::mysql::var const & rhs)
{
	// XXX the dynamic_cast avoid the cast issue when rhs is webpp::mysql::string
	return dynamic_cast<webpp::mysql::string const &>(rhs).get() == lhs;
}

#endif // HPP_MYSQL_VAR
