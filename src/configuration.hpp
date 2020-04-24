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
		configuration(int argc, char const * argv[]);

		std::map<std::string, std::string> const & get_environment() const
		{ return m_environment; }

        std::vector<std::string> const & get_args() const
        { return m_args; }

	private:
		std::vector<std::string> m_args;
		std::map<std::string, std::string> m_environment;
};

} // namespace webpp

#endif // HPP_CONFIGURATION_WEBPP
