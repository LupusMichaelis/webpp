#include "../src/query.hpp"

#include <boost/format.hpp>
#include <cgreen/cgreen.h>

using namespace cgreen;

Describe(query);

BeforeEach(query)
{
}

AfterEach(query)
{
}

Ensure(query, select_from_where_and_there)
{
	webpp::query::builder builder;

	auto query = builder
		.select({"id", "name"})
		.from("peoples")
		.where("id", "1")
		.and_("is_active", "true")
		;

	auto literal = query.str();
	assert_that(literal.c_str(),
			is_equal_to_string(
				"select `id`, `name` from `peoples`"
				" where `id` = 1"
				" and `is_active` = true"
				));
}
