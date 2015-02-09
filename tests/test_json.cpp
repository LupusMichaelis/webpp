
#include "../src/json.hpp"

#include <boost/format.hpp>
#include <iostream>
#include <sstream>
#include <cassert>

int main(int argc, char * argv[])
{
	std::vector<std::string> wellformed { "{}"
		, "[]"
		, "[[]]"
		, "{ \"array\": [1, \"rho\", 3, null, true, false, { \"boom\": 5, \"bam\": 2}] }"
		, "[null]"
		, "{\"null\":null}"
		, "[1,[]]"
		, "[1.2]"
		, "[-1.2]"
		, "[-1.2e23]"
		, "[1.2e23]"
		, "[1.2e+23]"
		, "[1.2e-23]"
	};

	for(auto s: wellformed)
	{
		std::cout << "===========================\n";
		std::cout << s;
		std::cout << "\n---------------------------\n";

		std::stringstream in {s};
		std::unique_ptr<webpp::json::value> p_tree;
		webpp::json::parse(p_tree, in);
		std::cout << "---------------------------\n";
		webpp::json::dump(std::cout, *p_tree);

		std::cout << "\n---------------------------\n";
		std::cout << "===========================" << std::endl;
	}

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

	for(auto s: badformed)
	{
		std::cout << "===========================\n";
		std::cout << s;
		std::cout << "\n---------------------------\n";

		std::stringstream in {s};
		std::unique_ptr<webpp::json::value> p_tree;
		std::cout << "---------------------------\n";
		try
		{
			webpp::json::parse(p_tree, in);
			assert(false);
		}
		catch(char const * p_e)
		{
			std::cout << "Caught";
		}

		std::cout << "\n---------------------------\n";
		std::cout << "===========================" << std::endl;
	}

	{
		std::string original {"{\"first\":true}"};
		std::stringstream in {original};
		std::stringstream out;
		{
			std::unique_ptr<webpp::json::value> p_tree;
			webpp::json::parse(p_tree, in);
			webpp::json::dump(out, *p_tree);
		}
		std::string parsed {out.str()};
		out.clear();

		std::unique_ptr<webpp::json::object> p_tree;
		build(p_tree);
		webpp::json::add_property(*p_tree, "first", true);
		std::stringstream out2;
		webpp::json::dump(out2, *p_tree);
		std::string generated {out2.str()};

		std::cout << "===========================\n";
		std::cout << (boost::format("%s:%d\n") % __FILE__ % __LINE__).str();
		std::cout << "---------------------------\n";
		std::cout << original;
		std::cout << "\n---------------------------\n";
		std::cout << generated;
		std::cout << "\n---------------------------\n";
		std::cout << "\n---------------------------\n";
		std::cout << parsed;
		std::cout << "\n---------------------------\n";
		assert(generated == parsed);
		std::cout << "===========================" << std::endl;
	}

	{
		std::string original {"[null,2,null,[]]"};
		std::stringstream in {original};
		std::stringstream out;
		{
			std::unique_ptr<webpp::json::value> p_tree;
			webpp::json::parse(p_tree, in);
			webpp::json::dump(out, *p_tree);
		}
		std::string parsed {out.str()};
		out.clear();

		std::unique_ptr<webpp::json::array> p_tree;
		build(p_tree);
		webpp::json::add(*p_tree, 1, 2);
		webpp::json::add(*p_tree, 3, webpp::json::array());
		std::stringstream out2;
		webpp::json::dump(out2, *p_tree);
		std::string generated {out2.str()};

		std::cout << "===========================\n";
		std::cout << (boost::format("%s:%d\n") % __FILE__ % __LINE__).str();
		std::cout << "---------------------------\n";
		std::cout << original;
		std::cout << "\n---------------------------\n";
		std::cout << generated;
		std::cout << "\n---------------------------\n";
		std::cout << "\n---------------------------\n";
		std::cout << parsed;
		std::cout << "\n---------------------------\n";
		assert(generated == parsed);
		std::cout << "===========================" << std::endl;
	}

	return 0;
}
