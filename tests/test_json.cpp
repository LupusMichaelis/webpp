
#include "../src/json.hpp"

#include <iostream>

#include <iostream>
#include <sstream>

int main(int argc, char * argv[])
{
	{
		std::stringstream in {"{}"};
		std::unique_ptr<webpp::json::value> p_tree;
		webpp::json::start(p_tree, in);
		webpp::json::dump(std::cout, *p_tree);
	}

	{
		std::stringstream in {"[]"};
		std::unique_ptr<webpp::json::value> p_tree;
		webpp::json::start(p_tree, in);
		webpp::json::dump(std::cout, *p_tree);
	}

	{
		std::stringstream in {"{ \"array\": [1, \"rho\", 3, null, true, false, { \"boom\": 1, \"bam\": 2}] }"};
		std::unique_ptr<webpp::json::value> p_tree;
		webpp::json::start(p_tree, in);
		webpp::json::dump(std::cout, *p_tree);
	}
}
