
#include "../src/http_request.hpp"
#include "../src/configuration.hpp"

#include <sstream>
#include <cgreen/cgreen.h>

extern char const ** environ;

static char const ** original;

using namespace cgreen;

Describe(http_request);

BeforeEach(http_request)
{
	original = environ;
	static char const * e[] = { "REQUEST_METHOD=GET", "REQUEST_URI=/", "CONTENT_TYPE=application/json", };
	environ = e;
}

AfterEach(http_request)
{
	environ = original;
}

Ensure(http_request, build_simple_request)
{
	webpp::configuration conf {0, NULL};
	std::unique_ptr<webpp::http::request> p_request;
	bool request_ok = webpp::http::from_cgi(conf, p_request);

	assert_that(request_ok, is_true);
	assert_that(p_request->method().c_str(), is_equal_to_string("GET"));
	assert_that(p_request->uri().c_str(), is_equal_to_string("/"));
	assert_that(p_request->content_type().c_str(), is_equal_to_string("application/json"));
}
