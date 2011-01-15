
#ifndef HPP_URL
#	define HPP_URL

#	include <string>

class Url
{
	public:
		virtual ~Url() ;
		Url() : m_raw() { }
		explicit Url(std::string const & raw) : m_raw(raw) { }

		std::string const & str() const ;
		std::string const str() ;

		virtual
		std::string const scheme() const ;
		virtual
		std::string const specific() const ;

	private:
		std::string m_raw ;

	protected:
		std::string & raw() ;
		std::string const & raw() const ;

		std::string::const_iterator seek_colon() const ;
} /* class Url */ ;

class HostUrl
	:public Url
{
	public:
		virtual ~HostUrl() ;
		HostUrl(Url const & copied);

		virtual
		std::string const host() const ;
} /* class HostUrl */ ;

class WebUrl
	: public HostUrl
{
	public:
		virtual ~WebUrl() ;
		WebUrl(Url const & copied);

} /* class WebUrl */ ;

class FtpUrl
	: public HostUrl
{
	public:
} /* class FtpUrl */ ;

#endif // HPP_URL
