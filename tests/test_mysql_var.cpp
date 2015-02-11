
#include "../src/mysql_var.hpp"

#include <boost/format.hpp>
#include <iostream>
#include <sstream>
#include <cassert>

class visitor;

int main(int argc, char * argv[])
{
	{
		webpp::mysql::string null;
		assert(null == nullptr);
		assert(nullptr == null);

		webpp::mysql::printer printer{std::cout};
		printer.visit(null);
	}

	{
		webpp::mysql::string my_subject;
		assert(my_subject == nullptr);
		assert(nullptr == my_subject);

		std::string subject {"Bonobo"};
		my_subject.set(subject);
		assert(nullptr != my_subject);
		assert(my_subject != nullptr);
		assert(subject == my_subject);
		assert(my_subject == subject);

		webpp::mysql::printer printer{std::cout};
		printer.visit(my_subject);
	}

	return 0;
}
