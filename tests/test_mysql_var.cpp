
#include "../src/mysql_var.hpp"

#include <iosfwd>
#include <sstream>
#include <cgreen/cgreen.h>

namespace {

using namespace webpp::mysql;

class printer : public visitor
{
	std::ostream & m_out;

	public:
		explicit printer(std::ostream & out)
			: m_out(out) {};

		virtual void visit(boolean & v)
		{
			if(nullptr == v)
				m_out << "null";
			else
				m_out << (v ? "true" : "false");
		}

		virtual void visit(string & v)
		{
			if(nullptr == v)
				m_out << "null";
			else
				m_out << v.get();
		}

		virtual void visit(integer & v)
		{
			if(nullptr == v)
				m_out << "null";
			else
				m_out << v.get();
		}

		virtual ~printer()
		{
		}
};
} // namespace

using namespace cgreen;

Ensure(null_object_is_null)
{
	webpp::mysql::string null;
	assert_that(null == nullptr, is_true);
	assert_that(nullptr == null, is_true);

	std::stringstream s;
	printer printer{s};
	printer.visit(null);

	std::string extract;
	s >> extract;

	assert_that(extract.c_str(), is_equal_to_string("null"));
}

Ensure(visit_string)
{
	webpp::mysql::string my_subject;
	assert_that(my_subject == nullptr, is_true);
	assert_that(nullptr == my_subject, is_true);

	std::string subject {"Bonobo"};
	my_subject.set(subject);
	assert_that(nullptr != my_subject, is_true);
	assert_that(my_subject != nullptr, is_true);
	assert_that(subject == my_subject, is_true);
	assert_that(my_subject == subject, is_true);

	std::stringstream s;
	printer printer{s};
	printer.visit(my_subject);

	std::string extract;
	s >> extract;

	assert_that(extract.c_str(), is_equal_to_string(subject.c_str()));
}

TestSuite *our_tests()
{
	TestSuite *suite = create_test_suite();
	add_test(suite, null_object_is_null);
	add_test(suite, visit_string);

	return suite;
}
