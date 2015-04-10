
#include "../src/http_response.hpp"

#include <cgreen/cgreen.h>

extern char const ** environ;

static char const ** original;

using namespace cgreen;

Describe(http_response);

BeforeEach(http_response)
{
}

AfterEach(http_response)
{
	environ = original;
}

Ensure(http_response, build_simple_response)
{
	webpp::http::response response;

	assert_that(response.status(), is_equal_to(200));
	assert_that(response.size(), is_equal_to(0));
	assert_that(response.body().c_str(), is_equal_to_string(""));

	response.status(404);
	assert_that(response.status(), is_equal_to(404));
}
