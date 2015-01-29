
#include "story.hpp"

#include <string>

struct story::impl
{
	std::string id;
	std::string title;
	std::string content;
	std::string modified;
	std::string created;
};

story::story()
	: mp_impl {std::make_unique<story::impl>()}
{
}

story::~story()
{
}

void story::set_id(std::string const & id)
{
	mp_impl->id = id;
}

void story::get_id(std::string & id)
{
	id = mp_impl->id;
}

void story::set_title(std::string const & title)
{
	mp_impl->title = title;
}

void story::get_title(std::string & title)
{
	title = mp_impl->title;
}

void story::set_content(std::string const & content)
{
	mp_impl->content = content;
}

void story::get_content(std::string & content)
{
	content = mp_impl->content;
}

void story::set_modified(std::string const & modified)
{
	mp_impl->modified = modified;
}

void story::get_modified(std::string & modified)
{
	modified = mp_impl->modified;
}

void story::set_created(std::string const & created)
{
	mp_impl->created = created;
}

void story::get_created(std::string & created)
{
	created = mp_impl->created;
}

