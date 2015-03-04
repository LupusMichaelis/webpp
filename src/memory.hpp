#ifndef HPP_MEMORY_WEBPP
#	define HPP_MEMORY_WEBPP

#	include <memory>

/// XXX Remove when STL will provide it
namespace std
{
	template<typename T, typename Del, typename ...Args>
	std::unique_ptr<T, Del> make_unique( Args&& ...args )
	{
		return std::unique_ptr<T, Del>( new T( std::forward<Args>(args)... ) );
	}

	template<typename T, typename ...Args>
	std::unique_ptr<T> make_unique( Args&& ...args )
	{
		return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
	}

}

template<typename T>
T & get_or_throw(std::unique_ptr<T> const & sptr)
{
	if(!sptr) throw std::bad_alloc() ;
	return *sptr ;
}

template<typename T>
T & get_or_throw(std::shared_ptr<T> const & sptr)
{
	if(!sptr) throw std::bad_alloc() ;
	return *sptr ;
}

#endif // HPP_MEMORY_WEBPP
