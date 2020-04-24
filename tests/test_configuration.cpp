#include "../src/configuration.hpp"

#include <sstream>
#include <cgreen/cgreen.h>

using namespace cgreen;

Describe(configuration);

BeforeEach(configuration)
{
}

AfterEach(configuration)
{
}

Ensure(configuration, no_element)
{
    char const * args[] = {};
	webpp::configuration conf {0, args};

	assert_that(conf.get_args().size(), is_equal_to(0));
}

Ensure(configuration, from_null)
{
	webpp::configuration conf {0, NULL};

	assert_that(conf.get_args().size(), is_equal_to(0));
}

Ensure(configuration, one_element)
{
    char const * args[] = {"toto"};
	webpp::configuration conf {1, args};

	assert_that(conf.get_args().size(), is_equal_to(1));
}
