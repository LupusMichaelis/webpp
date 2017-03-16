
#include "../src/router.hpp"

#include <boost/algorithm/string.hpp>

namespace webpp { namespace router {

actual::actual()
{
}

actual::~actual()
{
}

void actual::parse(std::string const & s)
{
	boost::split(m_segments, s, boost::is_any_of("/"));
}

};

} // namespace webpp::router
