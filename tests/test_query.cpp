#include "../src/query.hpp"
#include "../src/query_builder.hpp"

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

Ensure(query, select_all_from_table)
{
	webpp::query::escaper const escaper;
	webpp::query::simple_builder const builder {escaper};

	webpp::query::query query = builder
		.select()
		.from("familly")
		;

	auto literal = query.str();
	assert_that(literal.c_str(),
			is_equal_to_string("select * from `familly`"));
}

Ensure(query, select_from_where_and_there)
{
	webpp::query::escaper const escaper;
	webpp::query::simple_builder const builder {escaper};

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

Ensure(query, insert_into_table_field_values)
{
	webpp::query::escaper const escaper;
	webpp::query::simple_builder const builder {escaper};

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

Ensure(query, insert_into_table_values)
{
	webpp::query::escaper const escaper;
	webpp::query::simple_builder const builder {escaper};

	webpp::query::query query = builder
		.insert("familly")
		.values({{"Mickael"}, {"Ania"}, {"Dagmara"}})
		;

	auto literal = query.str();
	assert_that(literal.c_str(),
			is_equal_to_string(
				"insert `familly`"
				" values"
				" (\"Mickael\"), (\"Ania\"), (\"Dagmara\")"
				));
}

Ensure(query, replace)
{
	webpp::query::escaper const escaper;
	webpp::query::simple_builder const builder {escaper};

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
	webpp::query::simple_builder const builder {escaper};

	webpp::query::query query = builder
		.update("peoples")
		.set({{"is_enabled", "true"}})
		.where("id", "2")
		;

	auto literal = query.str();
	assert_that(literal.c_str(),
			is_equal_to_string(
				"update `peoples`"
				" set `is_enabled` = 'true'"
				" where `id` = 2"
				));
}

Ensure(query, visit_select_from_where_and_there)
{
	namespace wq = webpp::query;
	namespace wqc = wq::clause;
	namespace wqs = wq::schema;

	wqc::select	select;
	wqc::fields	fields	{{wqs::field{"toto"}}, false};
	wqc::from	from	{wqs::table{"toto"}};
	wqc::where	where	{wqs::field{"id"}, std::make_shared<wq::string>("1")};
	wqc::and_	and_	{wqs::field{"is_active"}, std::make_shared<wq::boolean>("false")};

	{
		auto step = wqc::checker<wqc::fields>(fields);

		step.visit(select);
		assert_that((bool) step, is_true);
		step.visit(fields);
		assert_that((bool) step, is_false);

		step.visit(from);
		assert_that((bool) step, is_false);
	}

	{
		auto step = wqc::checker<wqc::from>(from);

		step.visit(select);
		assert_that((bool) step, is_false);

		step.visit(fields);
		assert_that((bool) step, is_true);

		step.visit(from);
		assert_that((bool) step, is_false);
	}

	{
		auto step = wqc::checker<wqc::where>(where);

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
		auto step = wqc::checker<wqc::and_>(and_);

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

int main()
{
	query__select_from_where_and_there();
	query__insert_into_table_values();
}
