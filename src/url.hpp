#ifndef HPP_URL_WEBPP
#	define HPP_URL

#	include <string>

namespace webpp
{

class url
{
	public:
		virtual ~url();
		url() : m_raw() { }
		explicit url(std::string const & raw) : m_raw(raw) { }

		std::string const & str() const;
		std::string const str();

		virtual
		std::string const scheme() const;
		virtual
		std::string const specific() const;
		virtual
		std::string const host() const;

	private:
		std::string m_raw;

	protected:
		std::string & raw();
		std::string const & raw() const;

		std::string::const_iterator seek_colon() const;
};

} // namespace webpp

#endif // HPP_URL
