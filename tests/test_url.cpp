
#include "../src/url.hpp"

#include <sstream>
#include <cgreen/cgreen.h>

using namespace cgreen;

Ensure(web_url_is)
{
	std::string raw("http://lupusmic.org/");
	webpp::url perso(raw);

	assert_that(perso.str().c_str(), is_equal_to_string(raw.c_str()));
	assert_that(perso.specific().c_str(), is_equal_to_string("//lupusmic.org/"));

	assert_that(perso.host().c_str(), is_equal_to_string("lupusmic.org"));
}

TestSuite *our_tests()
{
	TestSuite *suite = create_test_suite();
	add_test(suite, web_url_is);

	return suite;
}
