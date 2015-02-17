
#include "../src/json.hpp"
#include "../src/json_parser.hpp"

#include <boost/format.hpp>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cgreen/cgreen.h>

using namespace cgreen;

Ensure(empty_object_is_wellformed)
{
	std::string s {"{}"};
	std::stringstream in {s};
	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	assert_that((bool) p_tree, is_true);
	assert_that(dynamic_cast<webpp::json::object&>(*p_tree).properties().size(), is_equal_to(0));
}

Ensure(empty_array_is_wellformed)
{
	std::string s {"[]"};
	std::stringstream in {s};
	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	assert_that((bool) p_tree, is_true);
	assert_that(dynamic_cast<webpp::json::array&>(*p_tree).values().size(), is_equal_to(0));
}

Ensure(nested_array_is_wellformed)
{
	std::string s {"[[]]"};
	std::stringstream in {s};
	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	assert_that((bool) p_tree, is_true);
	assert_that(dynamic_cast<webpp::json::array&>(*p_tree).values().size(), is_equal_to(1));
	assert_that(
		dynamic_cast<webpp::json::array&>(
			*dynamic_cast<webpp::json::array&>(*p_tree).values()[0])
				.values().size()
		, is_equal_to(0));
}

Ensure(array_with_null_is_wellformed)
{
	std::string s {"[null]"};
	std::stringstream in {s};
	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	assert_that((bool) p_tree, is_true);

	auto & a = dynamic_cast<webpp::json::array&>(*p_tree);
	assert_that(a.values().size(), is_equal_to(1));
	dynamic_cast<webpp::json::null&>(*a.values()[0]);
}

Ensure(object_with_null_is_wellformed)
{
	std::string s {"{\"null\":null}"};
	std::stringstream in {s};
	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	assert_that((bool) p_tree, is_true);

	auto & o = dynamic_cast<webpp::json::object&>(*p_tree);
	assert_that(o.properties().size(), is_equal_to(1));

	auto e = o.properties().cbegin();
	assert_that(e->first.c_str(), is_equal_to_string("null"));
	dynamic_cast<webpp::json::null&>(*e->second);
}

Ensure(array_number_empty_array_is_wellformed)
{
	std::string s {"[1,[]]"};
	std::stringstream in {s};
	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	assert_that((bool) p_tree, is_true);

	auto & a = dynamic_cast<webpp::json::array&>(*p_tree);
	assert_that(a.values().size(), is_equal_to(2));

	auto & e1 = dynamic_cast<webpp::json::number&>(*a.values()[0]);
	assert_that(e1.get().c_str(), is_equal_to_string("1"));

	auto & e2 = dynamic_cast<webpp::json::array&>(*a.values()[1]);
	assert_that(e2.values().size(), is_equal_to(0));
}

Ensure(object_complex_wellformed)
{
	std::string s {"{ \"array\": [1, \"rho\", 3, null, true, false, { \"boom\": 5, \"bam\": 2}] }"};
	std::stringstream in {s};
	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	assert_that((bool) p_tree, is_true);

	// TODO add more assertion on the elements
}

Ensure(array_contains_number)
{
	for(auto literal:
		{ "[1.2]"
		, "[-1.2]"
		, "[-1.2e23]"
		, "[1.2e23]"
		, "[1.2e+23]"
		, "[1.2e-23]"
		})
	{
		std::string s {literal};
		std::stringstream in {s};
		std::unique_ptr<webpp::json::value> p_tree;
		webpp::json::parse(p_tree, in);

		assert_that((bool) p_tree, is_true);

		auto & a = dynamic_cast<webpp::json::array&>(*p_tree);
		assert_that(a.values().size(), is_equal_to(1));

		auto & e = dynamic_cast<webpp::json::number&>(*a.values()[0]);
		std::string out;
		out += "[";
		out += e.get();
		out += "]";
		assert_that(out.c_str(), is_equal_to_string(s.c_str()));
	}
}

Ensure(literal_is_badformed)
{
	std::vector<std::string> badformed { "{{}"
		, "["
		, "[[]"
		, "{ \"array\" [] }"
		, "[,]"
		, "null"
		, "[+1]"
		, "[+1.2]"
		, "[--1.2e23]", "[-+1.2e23]", "[1.2e--23]", "[1.2e+-23]", "[1.2e-+23]", "[1.2e++23]", "[1.2e+23-]"
	};

	for(auto literal: badformed)
	{
		std::string s {literal};
		std::stringstream in {s};
		std::unique_ptr<webpp::json::value> p_tree;

		assert_throws(char *, webpp::json::parse(p_tree, in));
		assert_that((bool) p_tree, is_false);
	}
}

TestSuite *our_tests()
{
	TestSuite *suite = create_test_suite();
	add_test(suite, empty_array_is_wellformed);
	add_test(suite, nested_array_is_wellformed);
	add_test(suite, empty_object_is_wellformed);
	add_test(suite, array_with_null_is_wellformed);
	add_test(suite, object_with_null_is_wellformed);
	add_test(suite, array_number_empty_array_is_wellformed);
	add_test(suite, object_complex_wellformed);

	add_test(suite, literal_is_badformed);

	return suite;
}
