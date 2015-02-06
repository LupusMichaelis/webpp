
#include "../src/json.hpp"

#include <iostream>

class json_print : public webpp::json::visitor
{
	std::ostream &	m_out;
	mutable
	size_t			m_depth;

	public:

		explicit json_print(std::ostream & out) : m_out(out), m_depth(0) { };
		virtual ~json_print() { };

		void print(webpp::json::value const & node) const
		{
			node.accept(*this);
		}

		virtual void visit(webpp::json::array const & node) const
		{
			bool first = true;
			++m_depth;
			m_out << "[ ";
			for(auto const p_child: node.values())
			{
				if(first)
					first = false;
				else
					m_out << "\n" << std::string(m_depth - 1, '\t') << ", ";

				p_child->accept(*this);
			}
			m_out << "\n" << std::string(m_depth - 1, '\t') << "]";
			--m_depth;
		}

		virtual void visit(webpp::json::object const & node) const
		{
			bool first = true;
			++m_depth;
			m_out << "{ ";
			for(auto const child: node.properties())
			{
				if(first)
					first = false;
				else
					m_out << "\n" << std::string(m_depth - 1, '\t') << ", ";

				m_out << "\"" << child.first << "\": ";
				child.second->accept(*this);

			}
			m_out << "\n" << std::string(m_depth - 1, '\t') << "}";
			--m_depth;
		}

		virtual void visit(webpp::json::string const & node) const
		{
			m_out << "\"" << node.value() << "\"";
		}

		virtual void visit(webpp::json::number const & node) const
		{
			m_out << "\"" << node.value() << "\"";
		}

		virtual void visit(webpp::json::null const & node) const
		{
			m_out << "null";
		}

		virtual void visit(webpp::json::boolean const & node) const
		{
			m_out << (node.value() ? "true" : "false");
		}
};

#include <iostream>
#include <sstream>

int main(int argc, char * argv[])
{
	{
		std::stringstream in {"{}"};
		std::unique_ptr<webpp::json::value> p_tree;
		webpp::json::start(p_tree, in);
		json_print crawler {std::cout};
		crawler.print(*p_tree);
	}

	{
		std::stringstream in {"[]"};
		std::unique_ptr<webpp::json::value> p_tree;
		webpp::json::start(p_tree, in);
		json_print crawler {std::cout};
		crawler.print(*p_tree);
	}

	{
		std::stringstream in {"{ \"array\": [1, \"rho\", 3, null, true, false, { \"boom\": 1, \"bam\": 2}] }"};
		std::unique_ptr<webpp::json::value> p_tree;
		webpp::json::start(p_tree, in);
		json_print crawler {std::cout};
		crawler.print(*p_tree);
	}
}
