
#include "model.hpp"
#include "story.hpp"

#include <mysql.h>
#include <boost/format.hpp>

struct model::impl
{
	MYSQL * p_mysql;
};

model::model()
	: mp_impl {std::make_unique<model::impl>()}
{
	mp_impl->p_mysql = mysql_init(NULL);
	mysql_real_connect(mp_impl->p_mysql, "localhost", "test", "test", "test", 3308, NULL, 0);
}

model::~model()
{
	mysql_close(mp_impl->p_mysql);
}

void model::get_story_by_id(std::unique_ptr<story> & p_entity, std::string & id)
{
	std::unique_ptr<story> p_story {new story};
	p_story->set_id(id);

	auto query = boost::format("select title, content, created, modified from stories where id = %d") % id;

	if(mysql_query(mp_impl->p_mysql, query.str().c_str()))
		throw mysql_error(mp_impl->p_mysql);

	MYSQL_RES * result = NULL;
	result = mysql_store_result(mp_impl->p_mysql);
	if(1 != mysql_num_rows(result))
		throw "Too much or too many results";

	MYSQL_ROW row = NULL;
	row = mysql_fetch_row(result);

	p_story->set_title(row[0] ? row[0]: "");
	p_story->set_content(row[1] ? row[1]: "");
	p_story->set_created(row[2] ? row[2]: "");
	p_story->set_modified(row[3] ? row[3]: "");

	mysql_free_result(result);

	std::swap(p_entity, p_story);
}
