
#include "url.hpp"
#include <algorithm>

Url::~Url() { } ;
HostUrl::~HostUrl() { } ;
WebUrl::~WebUrl() { } ;

std::string const Url::str()
{
	return const_cast<std::string&>(const_cast<Url const &>(*this).str());
}

std::string const & Url::str() const
{
	return raw();
}

std::string & Url::raw()
{
	return const_cast<std::string &>(const_cast<Url const &>(*this).raw());
}

std::string const & Url::raw() const
{
	return m_raw;
}

std::string::const_iterator Url::seek_colon() const
{
	auto pos = std::find(str().begin(), str().end(), ':') ;
	if(pos == str().end())
		throw "URL doesn't have a scheme" ;

	return pos ;
}

std::string const Url::scheme() const
{
	return std::string(str().begin(), seek_colon()) ;
}

std::string const Url::specific() const
{
	return std::string(++seek_colon(), str().end()) ;
}

HostUrl::HostUrl(Url const & copied)
	: Url {copied}
{

}

std::string const HostUrl::host() const
{
	auto begin = seek_colon() ;

	std::string sep("://") ;
	std::advance(begin, sep.size()) ;
	if(begin == str().end())
		throw "Missing separator in URL" ;

	auto end = std::find(begin, str().end(), '/') ;

	return std::string(begin, end) ;
}

WebUrl::WebUrl(Url const & copied)
	: HostUrl {copied}
{

}
