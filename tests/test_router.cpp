
#include "../src/router.hpp"

#include <cgreen/cgreen.h>

using namespace cgreen;

Describe(router);

BeforeEach(router)
{
}

AfterEach(router)
{
}

Ensure(router, is_simple_route_with_two_segments)
{
	std::string route {"/segment-1/segment-2"};
	webpp::router::simple simple_router;
	std::unique_ptr<webpp::router::segment_list> p_segments;
	simple_router.parse(p_segments, route);

	assert_that(p_segments->size(), is_equal_to(3));
	assert_that(p_segments->at(0).c_str(), is_equal_to_string(""));
	assert_that(p_segments->at(1).c_str(), is_equal_to_string("segment-1"));
	assert_that(p_segments->at(2).c_str(), is_equal_to_string("segment-2"));
}
