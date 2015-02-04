#ifndef HPP_JSON_WEBPP
#	define HPP_JSON_WEBPP

#	include "memory.hpp"

#	include <vector>
#	include <map>
#	include <string>

namespace webpp { namespace json {

class parser
{
	struct impl;
	std::unique_ptr<impl> mp_impl;

	public:
		struct value
		{
			virtual void parse(char c, std::istream & in);

			virtual ~value();
		};

		struct object: value
		{
			std::map<std::string, std::shared_ptr<value>> m_properties;

			virtual void parse(char c, std::istream & in);
			virtual ~object();
		};

		struct array: value
		{
			std::vector<std::shared_ptr<value>> m_values;

			virtual void parse(char c, std::istream & in);
			virtual ~array();
		};

		struct root: value
		{
			std::shared_ptr<value> mp_node;

			virtual void parse(char c, std::istream & in);
			virtual ~root();
		};

		struct string: value
		{
			std::string m_value;

			virtual void parse(char c, std::istream & in);
			virtual ~string();
		};

		struct number: value
		{
			std::string m_value;

			virtual void parse(char c, std::istream & in);
			virtual ~number();
		};

		struct boolean: value
		{
			bool m_value;

			virtual void parse(char c, std::istream & in);
			virtual ~boolean();
		};

		struct null: value
		{
			virtual void parse(char c, std::istream & in);
			virtual ~null();
		};

	public:

		parser();
		~parser();

		void parse(std::unique_ptr<root> & p_tree, std::istream & in);
};

} } // namespace webpp::json

#endif // HPP_JSON_WEBPP
