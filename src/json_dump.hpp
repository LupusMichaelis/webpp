#ifndef HPP_DUMP_JSON_WEBPP
#	define HPP_DUMP_JSON_WEBPP

#	include "json.hpp"

namespace webpp { namespace json {

////////////////////////////////////////////////////////////////////////////////
// Dump JSON ///////////////////////////////////////////////////////////////////
void dump(std::ostream & out, webpp::json::value const & node);

class print
	: public const_visitor
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

} } // namespace webpp::json

#endif // HPP_DUMP_JSON_WEBPP
