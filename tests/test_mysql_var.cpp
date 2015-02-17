
#include "../src/mysql_var.hpp"

#include <sstream>
#include <cgreen/cgreen.h>

using namespace cgreen;

Ensure(null_object_is_null)
{
	webpp::mysql::string null;
	assert_that(null == nullptr, is_true);
	assert_that(nullptr == null, is_true);

	std::stringstream s;
	webpp::mysql::printer printer{s};
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
	webpp::mysql::printer printer{s};
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
