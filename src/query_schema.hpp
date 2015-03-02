#ifndef HPP_SCHEMA_QUERY_WEBPP
#	define HPP_SCHEMA_QUERY_WEBPP

#	include "query_var.hpp"

#	include "memory.hpp"
#	include <string>
/**
 * \todo boolean
 * \todo the operator == algorithm is flawed, need to review the logic
 */

namespace webpp { namespace query { namespace schema {

class type
{
	public:
		virtual ~type() { }
};

class table
{
	std::string m_name;

	public:
		explicit table(std::string name)
			: m_name {name}
		{ }

		std::string const & name() const { return m_name; }
};

class field
{
	std::string m_name;
	std::shared_ptr<var> mp_prototype;

	public:
		explicit field(std::string name)
			: m_name {name}
		{ }

		/*
		field(field const & copied)
			: m_name {copied.m_name}
			, mp_prototype {}
		{
			copied.clone(mp_prototype);
		}
		*/

		std::string const & name() const { return m_name; }

		bool const operator <(field const & rhs) const
		{
			return name() < rhs.name();
		}

		template<typename query_var_type>
		void of_type()
		{
			build<query_var_type>(mp_prototype);
		}

		void clone(std::shared_ptr<var> & p_clonee) const
		{
			mp_prototype->clone(p_clonee);
		}
};

} } } // namespace webpp::query::schema

#endif // HPP_SCHEMA_QUERY_WEBPP
