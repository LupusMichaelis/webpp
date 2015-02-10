#ifndef HPP_PARSER_JSON_WEBPP
#	define HPP_PARSER_JSON_WEBPP

#	include "memory.hpp"

#	include <vector>
#	include <map>
#	include <string>

namespace webpp { namespace json {

class value;

void parse(std::unique_ptr<value> & p_tree, std::istream & in);
void parse(std::unique_ptr<value> & p_value, char & c, std::istream & in);

class parser
{
	friend void parse(std::unique_ptr<value> & p_value, std::istream & in);

	struct impl;
	std::unique_ptr<impl> mp_impl;

	class value;

	public:
		parser();

		void parse(std::unique_ptr<json::value> & p_value, std::istream & in);
		void parse(std::unique_ptr<value> & p_value, std::istream & in);
		void parse(std::unique_ptr<value> & p_value, char & c, std::istream & in);

	private:

		class value
		{
			struct impl;
			std::unique_ptr<impl> mp_impl;

			protected:
				void assign_value(std::unique_ptr<json::value> & p_value);

			public:
				explicit value(parser & engine);
				value(value const & engine);

				json::value const & ref_value() const;
				parser & ref_parser() const;

				virtual bool first_condition(char c) = 0;
				virtual bool is_top_level() = 0;

				virtual void parse(char & c, std::istream & in) = 0;
				virtual void create(std::unique_ptr<value> & p_v) const = 0;
				virtual ~value();
		};

		class object: public value
		{
			public:
				explicit object(parser & engine);

				virtual bool first_condition(char c);
				virtual bool is_top_level();

				virtual void parse(char & c, std::istream & in);
				virtual void create(std::unique_ptr<value> & v) const;
				virtual ~object();
		};

		class array: public value
		{
			public:
				explicit array(parser & engine);

				virtual bool first_condition(char c);
				virtual bool is_top_level();

				virtual void parse(char & c, std::istream & in);
				virtual void create(std::unique_ptr<value> & v) const;
				virtual ~array();
		};

		class string: public value
		{
			public:
				explicit string(parser & engine);

				virtual bool first_condition(char c);
				virtual bool is_top_level();

				virtual void parse(char & c, std::istream & in);
				virtual void create(std::unique_ptr<value> & v) const;
				virtual ~string();
		};

		class number: public value
		{
			public:
				explicit number(parser & engine);

				virtual bool first_condition(char c);
				virtual bool is_top_level();

				virtual void parse(char & c, std::istream & in);
				virtual void create(std::unique_ptr<value> & v) const;
				virtual ~number();
		};

		class boolean: public value
		{
			public:
				explicit boolean(parser & engine);

				virtual bool first_condition(char c);
				virtual bool is_top_level();

				virtual void parse(char & c, std::istream & in);
				virtual void create(std::unique_ptr<value> & v) const;
				virtual ~boolean();
		};

		class null: public value
		{
			public:
				explicit null(parser & engine);

				virtual bool first_condition(char c);
				virtual bool is_top_level();

				virtual void parse(char & c, std::istream & in);
				virtual void create(std::unique_ptr<value> & v) const;
				virtual ~null();
		};
};

} } // namespace webpp::json

#endif // HPP_PARSER_JSON_WEBPP
