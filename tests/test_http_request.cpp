
#include "../src/http_request.hpp"

#include <sstream>
#include <cgreen/cgreen.h>

extern char const ** environ;

static char const ** original;

using namespace cgreen;

Describe(http_request);

BeforeEach(http_request)
{
	original = environ;
	static char const * e[] = { "REQUEST_METHOD=GET", "REQUEST_URI=/", };
	environ = e;
}

AfterEach(http_request)
{
	environ = original;
}

Ensure(http_request, build_simple_request)
{
	std::unique_ptr<webpp::http::request> p_request;
	webpp::http::from_cgi(p_request);

	assert_that(p_request->method().c_str(), is_equal_to_string("GET"));
	assert_that(p_request->uri().c_str(), is_equal_to_string("/"));
}
