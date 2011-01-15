
#include "../src/url.hpp"

#include <string>
#include <iostream>
#include <cassert>


int main()
{
	try
	{
		std::string raw("http://lupusmic.org/");
		Url perso(raw);

		assert(raw == perso.str());
		std::cout << perso.specific();
		assert(perso.specific() == "//lupusmic.org/");

		WebUrl website(perso);
		assert(website.host() == "lupusmic.org");

	}
	catch(std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch(std::string const & s)
	{
		std::cerr << s << std::endl;
	}
	catch(char const * msg)
	{
		std::cerr << msg << std::endl;
	}
}
