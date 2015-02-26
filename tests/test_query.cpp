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
		.from("familly")
		.where("id", "1")
		.and_("is_active", "true")
		;

	auto literal = query.str();
	assert_that(literal.c_str(),
			is_equal_to_string(
				"select `id`, `name` from `familly`"
				" where `id` = 1"
				" and `is_active` = true"
				));
}

Ensure(query, insert_into_table_values)
{
	webpp::query::escaper const escaper;
	webpp::query::builder const builder {escaper};

	webpp::query::query query = builder
		.insert("familly", {"name"})
		.values({{"Mickael"}, {"Ania"}, {"Dagmara"}})
		;

	auto literal = query.str();
	assert_that(literal.c_str(),
			is_equal_to_string(
				"insert `familly`"
				" (`name`)"
				" values"
				" (\"Mickael\"), (\"Ania\"), (\"Dagmara\")"
				));
}

Ensure(query, replace)
{
	webpp::query::escaper const escaper;
	webpp::query::builder const builder {escaper};

	webpp::query::query query = builder
		.replace("familly", {"name"})
		.values({{"Mickael"}, {"Ania"}, {"Dagmara"}})
		;

	auto literal = query.str();
	assert_that(literal.c_str(),
			is_equal_to_string(
				"replace `familly`"
				" (`name`)"
				" values"
				" (\"Mickael\"), (\"Ania\"), (\"Dagmara\")"
				));
}

Ensure(query, update)
{
	webpp::query::escaper const escaper;
	webpp::query::builder const builder {escaper};

	webpp::query::query query = builder
		.update("peoples")
		.set({{"is_enabled", "true"}})
		.where("id", "2")
		;

	auto literal = query.str();
	assert_that(literal.c_str(),
			is_equal_to_string(
				"update `peoples`"
				" set `is_enabled` = true"
				" where `id` = 2"
				));
}

Ensure(query, visit_select_from_where_and_there)
{
	webpp::query::clause::select	select;
	webpp::query::clause::fields	fields	{{"toto"}, false};
	webpp::query::clause::from		from	{"toto"};
	webpp::query::clause::where		where	{"id", "1"};
	webpp::query::clause::and_		and_	{"is_active", "false"};

	{
		auto step = webpp::query::clause::checker<webpp::query::clause::fields>(fields);

		step.visit(select);
		assert_that((bool) step, is_true);
		step.visit(fields);
		assert_that((bool) step, is_false);

		step.visit(from);
		assert_that((bool) step, is_false);
	}

	{
		auto step = webpp::query::clause::checker<webpp::query::clause::from>(from);

		step.visit(select);
		assert_that((bool) step, is_false);

		step.visit(fields);
		assert_that((bool) step, is_true);

		step.visit(from);
		assert_that((bool) step, is_false);
	}

	{
		auto step = webpp::query::clause::checker<webpp::query::clause::where>(where);

		step.visit(select);
		assert_that((bool) step, is_false);

		step.visit(fields);
		assert_that((bool) step, is_false);

		step.visit(from);
		assert_that((bool) step, is_true);

		step.visit(where);
		assert_that((bool) step, is_false);
	}

	{
		auto step = webpp::query::clause::checker<webpp::query::clause::and_>(and_);

		step.visit(select);
		assert_that((bool) step, is_false);

		step.visit(fields);
		assert_that((bool) step, is_false);

		step.visit(from);
		assert_that((bool) step, is_false);

		step.visit(where);
		assert_that((bool) step, is_true);

		step.visit(and_);
		assert_that((bool) step, is_true); // can chain
	}
}
