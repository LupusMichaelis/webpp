
#include "url.hpp"
#include <algorithm>

namespace webpp
{

url::~url() { };

std::string const url::str()
{
	return const_cast<std::string&>(const_cast<url const &>(*this).str());
}

std::string const & url::str() const
{
	return raw();
}

std::string & url::raw()
{
	return const_cast<std::string &>(const_cast<url const &>(*this).raw());
}

std::string const & url::raw() const
{
	return m_raw;
}

std::string::const_iterator url::seek_colon() const
{
	auto pos = std::find(str().begin(), str().end(), ':');
	if(pos == str().end())
		throw "URL doesn't have a scheme";

	return pos;
}

std::string const url::scheme() const
{
	return std::string(str().begin(), seek_colon());
}

std::string const url::specific() const
{
	return std::string(++seek_colon(), str().end());
}

std::string const url::host() const
{
	auto begin = seek_colon();

	std::string sep("://");
	std::advance(begin, sep.size());
	if(begin == str().end())
		throw "Missing separator in URL";

	auto end = std::find(begin, str().end(), '/');

	return std::string(begin, end);
}

}; // namespace webpp
