
#include "../src/router.hpp"

#include <boost/algorithm/string.hpp>

namespace webpp { namespace router {

simple::simple()
{
}

simple::~simple()
{
}

void simple::parse(std::unique_ptr<segment_list> & p_segments, std::string const & s) const
{
	segment_list segments;
	boost::split(segments, s, boost::is_any_of("/"));
	p_segments = std::move(std::make_unique<segment_list>(segments));
}

};

} // namespace webpp::router
