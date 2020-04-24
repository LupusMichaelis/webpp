#include "../src/http_request.hpp"
#include "../src/configuration.hpp"

#include <sstream>
#include <cgreen/cgreen.h>

extern char const ** environ;

using namespace cgreen;

Describe(http_request);

BeforeEach(http_request)
{
	setenv("REQUEST_METHOD", "GET", 1);
	setenv("REQUEST_URI", "/", 1);
	setenv("CONTENT_TYPE", "application/json", 1);
}

AfterEach(http_request)
{
}

Ensure(http_request, build_simple_request)
{

	char const * args[] = {};
	webpp::configuration conf {0, args};
	assert_that(conf.get_args().size(), is_equal_to(0));
	std::unique_ptr<webpp::http::request> p_request;

	bool request_ok = webpp::http::from_cgi(conf, p_request);

	assert_that(request_ok, is_true);
	assert_that(p_request->method().c_str(), is_equal_to_string("GET"));
	assert_that(p_request->uri().c_str(), is_equal_to_string("/"));
	assert_that(p_request->content_type().c_str(), is_equal_to_string("application/json"));
}
