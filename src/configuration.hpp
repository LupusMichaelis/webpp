#ifndef HPP_CONFIGURATION_WEBPP
#	define HPP_CONFIGURATION_WEBPP

#	include <map>
#	include <string>
#	include <vector>

namespace webpp
{

class configuration
{
	public:
		configuration(int argc, char * argv[]);

		std::map<std::string, std::string> const & get_environment() const
		{ return m_environment; }

	private:
		std::vector<std::string> m_args;
		std::map<std::string, std::string> m_environment;
};

} // namespace webpp

#endif // HPP_CONFIGURATION_WEBPP
