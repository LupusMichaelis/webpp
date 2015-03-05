#ifndef HPP_JSON_WEBPP
#	define HPP_JSON_WEBPP

#	include "memory.hpp"

#	include <vector>
#	include <map>
#	include <string>
#	include <boost/format.hpp>


/**
 * \todo line/position counting while parsing
 * \todo proper exceptions
 * \todo characters as constant?
 * \todo support well-formed strings
 * \todo support date detection and formatting
 *
 */

namespace webpp { namespace json {

class value;
class string;
class array;
class object;
class number;
class null;
class boolean;

void dump(std::ostream & out, webpp::json::value const & node);

template <typename value_type>
void build(std::unique_ptr<value_type> & p_node);

extern template void build<string>(std::unique_ptr<string> & p_node);
extern template void build<array>(std::unique_ptr<array> & p_node);
extern template void build<object>(std::unique_ptr<object> & p_node);
extern template void build<number>(std::unique_ptr<number> & p_node);
extern template void build<null>(std::unique_ptr<null> & p_node);
extern template void build<boolean>(std::unique_ptr<boolean> & p_node);

template <typename storage_type>
void build(std::unique_ptr<value> & p_node);

extern template void build<null>(std::unique_ptr<value> & p_node);

template <typename value_type, typename native_value>
void build(std::unique_ptr<value_type> & p_node, native_value const & value);

extern template void build<string, std::string>(std::unique_ptr<string> & p_node, std::string const & value);
extern template void build<boolean, bool>(std::unique_ptr<boolean> & p_node, bool const & value);
extern template void build<number, std::string>(std::unique_ptr<number> & p_node, std::string const & value);

template <typename value_type, typename native_value>
void build(std::unique_ptr<value> & p_node, native_value const & value);

extern template void build<string, std::string>(std::unique_ptr<value> & p_node, std::string const & value);
extern template void build<boolean, bool>(std::unique_ptr<value> & p_node, bool const & value);
extern template void build<number, std::string>(std::unique_ptr<value> & p_node, std::string const & value);

class visitor
{
	public:
		virtual void visit(webpp::json::string const & node) const = 0;
		virtual void visit(webpp::json::array const & node) const = 0;
		virtual void visit(webpp::json::object const & node) const = 0;
		virtual void visit(webpp::json::number const & node) const = 0;
		virtual void visit(webpp::json::null const & node) const = 0;
		virtual void visit(webpp::json::boolean const & node) const = 0;
		virtual ~visitor();
};

class print
	: public visitor
{
	struct impl;
	std::unique_ptr<impl> mp_impl;

	public:

		print() = delete;
		explicit print(std::ostream & out);
		virtual ~print();

		virtual void visit(webpp::json::string const & node) const;
		virtual void visit(webpp::json::array const & node) const;
		virtual void visit(webpp::json::object const & node) const;
		virtual void visit(webpp::json::number const & node) const;
		virtual void visit(webpp::json::null const & node) const;
		virtual void visit(webpp::json::boolean const & node) const;
};

class value
{
	public:
		virtual void accept(visitor const & v) const = 0;
		virtual void clone(std::unique_ptr<value> & p_v) const = 0;
		virtual ~value();
};

class object: public value
{
	std::map<std::string, std::shared_ptr<value>> m_properties;

	public:
		object() = default;
		object(object const &);

		virtual void accept(visitor const & v) const;
		virtual void clone(std::unique_ptr<value> & p_v) const;
		virtual ~object();

		std::map<std::string, std::shared_ptr<value>> const & properties() const { return m_properties; };
		void add_property(std::string const key, std::unique_ptr<value> & node);
};

void add_property(object & self, std::string const key, std::unique_ptr<value> & p_node);
void add_property(object & self, std::string const key, std::unique_ptr<object> & p_object);
void add_property(object & self, std::string const key, std::unique_ptr<array> & p_array);
void add_property(object & self, std::string const key, bool const value);
void add_property(object & self, std::string const key, std::string const value);
void add_property(object & self, std::string const key, std::nullptr_t const );

class array: public value
{
	std::vector<std::shared_ptr<value>> m_values;

	public:
		array() = default;
		array(array const &);

		virtual void accept(visitor const & v) const;
		virtual void clone(std::unique_ptr<value> & p_v) const;
		virtual ~array();

		std::vector<std::shared_ptr<value>> const & values() const { return m_values; };
		void add(std::unique_ptr<value> & p_node);
		void add(size_t index, std::unique_ptr<value> & p_node);
};

void add(array & self, std::unique_ptr<value> & p_node);
void add(array & self, std::unique_ptr<object> & p_object);
void add(array & self, std::unique_ptr<array> & p_array);
void add(array & self, bool const value);
void add(array & self, std::string const value);
void add(array & self, std::string const key, std::nullptr_t const );

void add(array & self, size_t index, int const number_value);
void add(array & self, size_t index, array const array_value);
void add(array & self, std::string const key, array const array_value);

class parser;

class string
	: public value
{
	std::string m_value;

	public:
		virtual void accept(visitor const & v) const;
		virtual void clone(std::unique_ptr<value> & p_v) const;
		virtual ~string();

		std::string const & get() const	{ return m_value; };
		void set(std::string new_value)	{ m_value = new_value; };
};

class number
	: public value
{
	std::string m_value;

	public:
		virtual void accept(visitor const & v) const;
		virtual void clone(std::unique_ptr<value> & p_v) const;
		virtual ~number();

		std::string const & get() const { return m_value; };
		void set(std::string const new_value)	{ m_value = new_value; };
		void set(int const new_value)	{ m_value = (boost::format("%d") % new_value).str(); };
};

class boolean
	: public value
{
	bool m_value;

	public:
		virtual void accept(visitor const & v) const;
		virtual void clone(std::unique_ptr<value> & p_v) const;
		virtual ~boolean();

		bool const get() const	{ return m_value;};
		void set(bool new_value)	{ m_value = new_value; };
};

class null
	: public value
{
	public:
		virtual void accept(visitor const & v) const;
		virtual void clone(std::unique_ptr<value> & p_v) const;
		virtual ~null();
};

} } // namespace webpp::json

#endif // HPP_JSON_WEBPP
