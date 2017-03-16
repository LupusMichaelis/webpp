#ifndef HPP_ROUTER_WEBPP
#	define HPP_ROUTER_WEBPP

#	include "memory.hpp"

#	include <vector>
#	include <string>

namespace webpp {
namespace router {

typedef std::vector<std::string> segment_list;

class actual
{
	public:
		actual();
		~actual();

		void parse(std::string const & s);

		bool is_root() const
		{
			return m_segments.size() == 1;
		}

		void get_segment(std::string & segment, size_t const idx) const
		{
			segment = m_segments.at(idx);
		}

		bool has_segment(size_t const idx) const
		{
			return idx < m_segments.size();
		}

		size_t get_segment_count() const
		{
			return m_segments.size();
		}

	private:
		segment_list m_segments;
};

} // namespace router
} // namespace webpp

#endif // HPP_ROUTER_WEBPP
