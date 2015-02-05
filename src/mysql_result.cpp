#include "mysql_result.hpp"

namespace webpp { namespace mysql {

struct result::impl
{
	field_list_type	m_field_list;
	row_list_type	m_row_list;
	size_t			m_updated_rows;
};

result::result()
	: mp_impl {std::make_unique<impl>()}
{
}

result::~result()
{
}

result::operator bool() const
{
	return true;
}

void result::fields(field_list_type field_list)
{
	mp_impl->m_field_list = field_list;
}

void result::rows(row_list_type row_list)
{
	mp_impl->m_row_list = row_list;
}

void result::fields(std::unique_ptr<field_list_type> & p_field_list) const
{
	auto p = std::make_unique<field_list_type>(mp_impl->m_field_list);
	std::swap(p, p_field_list);
}

void result::rows(std::unique_ptr<row_list_type> & p_row_list) const
{
	auto p = std::make_unique<row_list_type>(mp_impl->m_row_list);
	std::swap(p, p_row_list);
}

void result::updated_rows(size_t update_rows)
{
	mp_impl->m_updated_rows = update_rows;
}

size_t result::updated_rows() const
{
	return mp_impl->m_updated_rows;
}

} } // namespace webpp::mysql
