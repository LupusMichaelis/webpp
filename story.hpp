#ifndef HPP_CUSTOMER
#	define HPP_CUSTOMER

#	include "memory.hpp"

class story
{
	class impl;
	std::unique_ptr<impl> mp_impl;

	public:
		story();
		~story();

		void set_id(std::string const & );
		void set_title(std::string const & );
		void set_content(std::string const & );
		void set_created(std::string const & );
		void set_modified(std::string const & );

		void get_id(std::string & );
		void get_title(std::string & );
		void get_content(std::string & );
		void get_created(std::string & );
		void get_modified(std::string & );
};

#endif // HPP_CUSTOMER

