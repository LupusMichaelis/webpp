
#include "../src/json.hpp"
#include "../src/json_parser.hpp"

#include <boost/format.hpp>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cgreen/cgreen.h>

using namespace cgreen;

Describe(json);

BeforeEach(json)
{
}

AfterEach(json)
{
}

Ensure(json, empty_object_is_wellformed)
{
	std::string s {"{}"};
	std::stringstream in {s};
	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	assert_that((bool) p_tree, is_true);
	assert_that(dynamic_cast<webpp::json::object&>(*p_tree).properties().size(), is_equal_to(0));
}

Ensure(json, empty_array_is_wellformed)
{
	std::string s {"[]"};
	std::stringstream in {s};
	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	assert_that((bool) p_tree, is_true);
	assert_that(dynamic_cast<webpp::json::array&>(*p_tree).values().size(), is_equal_to(0));
}

Ensure(json, nested_array_is_wellformed)
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

Ensure(json, array_with_null_is_wellformed)
{
	std::string s {"[null]"};
	std::stringstream in {s};
	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	assert_that((bool) p_tree, is_true);

	auto & a = dynamic_cast<webpp::json::array&>(*p_tree);
	assert_that(a.values().size(), is_equal_to(1));
	(void) dynamic_cast<webpp::json::null&>(*a.values()[0]);
}

Ensure(json, object_with_null_is_wellformed)
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
	(void) dynamic_cast<webpp::json::null&>(*e->second);
}

Ensure(json, array_number_empty_array_is_wellformed)
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

Ensure(json, object_complex_wellformed)
{
	std::string s {"{ \"array\": [1, \"rho\", 3, null, true, false, { \"boom\": 5, \"bam\": 2}] }"};
	std::stringstream in {s};
	std::unique_ptr<webpp::json::value> p_tree;
	webpp::json::parse(p_tree, in);

	assert_that((bool) p_tree, is_true);

	// TODO add more assertion on the elements
}

Ensure(json, array_contains_number)
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

Ensure(json, literal_is_badformed)
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

Ensure(json, parsed_equals_generated)
{
	std::string original {"{\"first\":true}"};
	std::stringstream in {original};
	std::stringstream dump_original;
	{
		std::unique_ptr<webpp::json::value> p_tree;
		webpp::json::parse(p_tree, in);
		webpp::json::dump(dump_original, *p_tree);
	}
	std::string parsed {dump_original.str()};

	std::unique_ptr<webpp::json::object> p_tree;
	build(p_tree);
	webpp::json::add_property(*p_tree, "first", true);
	std::stringstream dump_generated;
	webpp::json::dump(dump_generated, *p_tree);
	std::string generated {dump_generated.str()};

	assert_that(generated.c_str(), is_equal_to_string(parsed.c_str()));
}

Ensure(json, parsed_equals_generated_with_holes)
{
	std::string original {"[null,2,null,[]]"};
	std::stringstream in {original};
	std::stringstream dump_original;
	{
		std::unique_ptr<webpp::json::value> p_tree;
		webpp::json::parse(p_tree, in);
		webpp::json::dump(dump_original, *p_tree);
	}
	std::string parsed {dump_original.str()};

	std::unique_ptr<webpp::json::array> p_tree;
	build(p_tree);
	webpp::json::add(*p_tree, 1, 2);
	webpp::json::add(*p_tree, 3, webpp::json::array());
	std::stringstream dump_generated;
	webpp::json::dump(dump_generated, *p_tree);
	std::string generated {dump_generated.str()};

	assert_that(generated.c_str(), is_equal_to_string(parsed.c_str()));
}
