
#include "../src/url.hpp"

#include <sstream>
#include <cgreen/cgreen.h>

using namespace cgreen;

Describe(url);

BeforeEach(url)
{
}

AfterEach(url)
{
}

Ensure(url, web_url_is)
{
	std::string raw("http://lupusmic.org/");
	webpp::url perso(raw);

	assert_that(perso.str().c_str(), is_equal_to_string(raw.c_str()));
	assert_that(perso.specific().c_str(), is_equal_to_string("//lupusmic.org/"));

	assert_that(perso.host().c_str(), is_equal_to_string("lupusmic.org"));
}
