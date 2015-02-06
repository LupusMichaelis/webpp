
#include "../src/json.hpp"

#include <iostream>

class json_print : public webpp::json::visitor
{
	std::ostream & m_out;

	public:

		json_print();
		explicit json_print(std::ostream & out) : m_out(out) { };
		virtual ~json_print() { };

		void print(webpp::json::value const & node) const
		{
			node.visit(*this);
		}

		virtual void visit(webpp::json::array const & node) const
		{
			m_out << "Array\n";
			for(auto const p_child: node.values())
				p_child->visit(*this);
		}

		virtual void visit(webpp::json::object const & node) const
		{
			m_out << "Object\n";
			for(auto const child: node.properties())
			{
				m_out << "[" << child.first << "] ";
				child.second->visit(*this);
			}
		}

		virtual void visit(webpp::json::string const & node) const
		{
			m_out << "String\n";
		}

		virtual void visit(webpp::json::number const & node) const
		{
			m_out << "Number\n";
		}

		virtual void visit(webpp::json::null const & node) const
		{
			m_out << "null\n";
		}

		virtual void visit(webpp::json::boolean const & node) const
		{
			m_out << (node.value() ? "true" : "false") << "\n";
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
