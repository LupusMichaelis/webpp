#ifndef HPP_JSON_WEBPP
#	define HPP_JSON_WEBPP

#	include "memory.hpp"

#	include <vector>
#	include <map>
#	include <string>

namespace webpp { namespace json {

class value;
class string;
class array;
class object;
class number;
class null;
class boolean;

void start(std::unique_ptr<value> & p_tree, std::istream & in);
void parse(std::unique_ptr<value> & p_value, char c, std::istream & in);
void dump(std::ostream & out, webpp::json::value const & node);

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

class print : public visitor
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
		virtual void parse(char c, std::istream & in) = 0;
		virtual void accept(visitor const & v) const = 0;
		virtual ~value();
};

class object: public value
{
	std::map<std::string, std::shared_ptr<value>> m_properties;

	public:
		virtual void parse(char c, std::istream & in);
		virtual void accept(visitor const & v) const;
		virtual ~object();

		std::map<std::string, std::shared_ptr<value>> const & properties() const { return m_properties; };
};

class array: public value
{
	std::vector<std::shared_ptr<value>> m_values;

	public:
		virtual void parse(char c, std::istream & in);
		virtual void accept(visitor const & v) const;
		virtual ~array();

		std::vector<std::shared_ptr<value>> const & values() const { return m_values; };
};

class string: public value
{
	std::string m_value;

	public:
		virtual void parse(char c, std::istream & in);
		virtual void accept(visitor const & v) const;
		virtual ~string();

		std::string const & value() const { return m_value; };
};

class number: public value
{
	std::string m_value;

	public:
		virtual void parse(char c, std::istream & in);
		virtual void accept(visitor const & v) const;
		virtual ~number();

		std::string const & value() const { return m_value; };
};

class boolean: public value
{
	bool m_value;

	public:
		virtual void parse(char c, std::istream & in);
		virtual void accept(visitor const & v) const;
		virtual ~boolean();

		bool const value() const { return m_value;};
};

class null: public value
{
	public:
		virtual void parse(char c, std::istream & in);
		virtual void accept(visitor const & v) const;
		virtual ~null();
};

} } // namespace webpp::json

#endif // HPP_JSON_WEBPP
