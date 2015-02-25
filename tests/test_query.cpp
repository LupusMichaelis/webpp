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
	webpp::query::escaper const escaper;
	webpp::query::builder const builder {escaper};

	webpp::query::query query = builder
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

Ensure(query, insert_into_table_values)
{
	webpp::query::escaper const escaper;
	webpp::query::builder const builder {escaper};

	webpp::query::query query = builder
		.insert("peoples", {"name"})
		.values({{"Mickael"}, {"Ania"}, {"Dagmara"}})
		;

	auto literal = query.str();
	assert_that(literal.c_str(),
			is_equal_to_string(
				"insert into `peoples`"
				" (`name`)"
				" values"
				" (\"Mickael\"), (\"Ania\"), (\"Dagmara\")"
				));
}
