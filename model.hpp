#ifndef HPP_MODEL
#	define HPP_MODEL

#	include "memory.hpp"

class story;

class model
{
	class impl;
	std::unique_ptr<impl> mp_impl;

	public:
		model();
		~model();

		void get_story_by_id(std::unique_ptr<story> & p_entity, std::string & id);
};

#endif // HPP_MODEL
