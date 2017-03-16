
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
	webpp::router::actual simple_router;
	simple_router.parse(route);

	assert_that(simple_router.get_segment_count(), is_equal_to(3));
	std::string segment;
	simple_router.get_segment(segment, 0);
	assert_that(segment.c_str(), is_equal_to_string(""));
	simple_router.get_segment(segment, 1);
	assert_that(segment.c_str(), is_equal_to_string("segment-1"));
	simple_router.get_segment(segment, 2);
	assert_that(segment.c_str(), is_equal_to_string("segment-2"));
}
