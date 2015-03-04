
#include "../src/query_var.hpp"

#include <iosfwd>
#include <sstream>
#include <cgreen/cgreen.h>

namespace {

using namespace webpp::query;

class printer
	: public const_visitor
{
	std::ostream & m_out;

	public:
		explicit printer(std::ostream & out)
			: m_out(out) {};

		virtual void visit(boolean const & v)
		{
			using namespace webpp::query::comparison;

			if(nullptr == v)
				m_out << "null";
			else
				m_out << (true == v ? "TRUE" : "FALSE");
		}

		virtual void visit(string const & v)
		{
			using namespace webpp::query::comparison;

			if(nullptr == v)
				m_out << "NULL";
			else
				m_out << v.get();
		}

		virtual void visit(integer const & v)
		{
			using namespace webpp::query::comparison;

			if(nullptr == v)
				m_out << "NULL";
			else
				m_out << v.get();
		}

		virtual ~printer()
		{
		}
};
} // namespace

using namespace cgreen;

Describe(mysql_var);

BeforeEach(mysql_var)
{
}

AfterEach(mysql_var)
{
}

Ensure(mysql_var, null_object_is_null)
{
	using namespace webpp::query::comparison;

	webpp::query::string null;
	assert_that(null == nullptr, is_true);
	assert_that(nullptr == null, is_true);

	std::stringstream s;
	printer printer{s};
	printer.visit(null);

	std::string extract;
	s >> extract;

	assert_that(extract.c_str(), is_equal_to_string("NULL"));
}

Ensure(mysql_var, visit_string)
{
	using namespace webpp::query::comparison;

	webpp::query::string my_subject;
	assert_that(my_subject == nullptr, is_true);
	assert_that(nullptr == my_subject, is_true);

	std::string subject {"Bonobo"};
	my_subject.set(subject);
	assert_that(nullptr != my_subject, is_true);
	assert_that(my_subject != nullptr, is_true);
	assert_that(subject == my_subject, is_true);
	assert_that(my_subject == subject, is_true);

	std::stringstream s;
	printer p{s};
	p.visit(my_subject);

	std::string extract;
	s >> extract;

	assert_that(extract.c_str(), is_equal_to_string(subject.c_str()));
}

Ensure(mysql_var, visit_boolean)
{
	using namespace webpp::query::comparison;

	webpp::query::boolean my_truth;
	assert_that(my_truth == nullptr, is_true);
	assert_that(nullptr == my_truth, is_true);

	assert_that(true == my_truth, is_false);
	assert_that(false == my_truth, is_false);

	my_truth = true;
	assert_that(my_truth == nullptr, is_false);
	assert_that(nullptr == my_truth, is_false);
	assert_that(my_truth == true, is_true);
	assert_that(true == my_truth, is_true);

	std::string literal {"TRUE"};
	std::stringstream s;
	printer p{s};
	p.visit(my_truth);

	std::string extract;
	s >> extract;

	assert_that(extract.c_str(), is_equal_to_string(literal.c_str()));
}

Ensure(mysql_var, compare_different_types)
{
	using namespace webpp::query::comparison;

	webpp::query::boolean my_truth;
	webpp::query::string my_subject;

	// null objects
	assert_that(my_subject == my_truth, is_true);
	assert_that(my_truth == my_subject, is_true);

	//
	my_truth = true;
	assert_that(nullptr == my_truth, is_false);
	assert_that(true == my_truth, is_true);
	assert_that(my_subject == my_truth, is_false);
	assert_that(my_truth == my_subject, is_false);

	//
	my_subject = "random";
	assert_that(nullptr == my_subject, is_false);
	assert_that(std::string("random") == my_subject, is_true);
	assert_that(my_subject == my_truth, is_false);
	assert_that(my_truth == my_subject, is_false);
}
