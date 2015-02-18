#ifndef HPP_ROUTER_WEBPP
#	define HPP_ROUTER_WEBPP

#	include "memory.hpp"

#	include <vector>
#	include <string>

namespace webpp {
namespace router {

typedef std::vector<std::string> segment_list;

class simple
{
	public:
		simple();
		~simple();

		void parse(std::unique_ptr<segment_list> & p_segments, std::string const & s) const;
};

} // namespace router
} // namespace webpp

#endif // HPP_ROUTER_WEBPP
