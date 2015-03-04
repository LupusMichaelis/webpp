#ifndef HPP_ALGORITHM_WEBPP
#	define HPP_ALGORITHM_WEBPP

#	include <algorithm>

/// XXX Remove when STL will provide it
namespace webpp {
	template<typename Container, typename Predicate>
	typename Container::iterator sort(Container & container, Predicate predicate)
	{
		return std::sort(std::begin(container), std::end(container), predicate);
	}

	template<typename Container, typename Predicate>
	typename Container::const_iterator find_if(Container const & container, Predicate predicate)
	{
		return std::find_if(std::begin(container), std::end(container), predicate);
	}

	template<typename Container, typename Predicate>
	void for_each(Container const & container, Predicate predicate)
	{
		std::for_each(std::begin(container), std::end(container), predicate);
	}
} // namespace webpp

namespace std {
	using webpp::sort;
	using webpp::find_if;
	using webpp::for_each;
}

#endif // HPP_ALGORITHM_WEBPP
