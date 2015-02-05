#ifndef HPP_JSON_WEBPP
#	define HPP_JSON_WEBPP

#	include "memory.hpp"

#	include <vector>
#	include <map>
#	include <string>

namespace webpp { namespace json {

class value;

void start(std::unique_ptr<value> & p_tree, std::istream & in);
void parse(std::unique_ptr<value> & p_value, char c, std::istream & in);

class value
{
	public:
		virtual void parse(char c, std::istream & in) = 0;
		virtual ~value();
};

class object: public value
{
	std::map<std::string, std::shared_ptr<value>> m_properties;

	public:
		virtual void parse(char c, std::istream & in);
		virtual ~object();

		std::map<std::string, std::shared_ptr<value>> const & properties() const { return m_properties; };
};

class array: public value
{
	std::vector<std::shared_ptr<value>> m_values;

	public:
		virtual void parse(char c, std::istream & in);
		virtual ~array();

		std::vector<std::shared_ptr<value>> const & values() const;
};

class string: public value
{
	std::string m_value;

	public:
		virtual void parse(char c, std::istream & in);
		virtual ~string();

		std::string const & value() const { return m_value; };
};

class number: public value
{
	std::string m_value;

	public:
		virtual void parse(char c, std::istream & in);
		virtual ~number();

		std::string const & value() const { return m_value; };
};

class boolean: public value
{
	bool m_value;

	public:
		virtual void parse(char c, std::istream & in);
		virtual ~boolean();

		bool const value() const { return m_value;};
};

class null: public value
{
	public:
		virtual void parse(char c, std::istream & in);
		virtual ~null();
};

} } // namespace webpp::json

#endif // HPP_JSON_WEBPP
