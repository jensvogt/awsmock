
#include <awsmock/core/XmlUtils.h>

namespace AwsMock::Core {

    std::string XmlUtils::ToXmlString(const boost::property_tree::ptree &rootTree) {
        std::stringstream os;
        write_xml(os, rootTree);
        return os.str();
    }

    void XmlUtils::ReadXml(const std::string &xmlString, boost::property_tree::ptree *rootTree) {
        std::stringstream is;
        is << xmlString;
        read_xml(is, *rootTree);
    }

    std::string XmlUtils::Indent(const int level) {
        std::string s;
        for (int i = 0; i < level; i++) s += "  ";
        return s;
    }

    void XmlUtils::PrintTree(const boost::property_tree::ptree &rootTree, const int level) {
        if (rootTree.empty()) {
            std::cout << "\"" << rootTree.data() << "\"";
        }

        else {
            if (level) std::cout << std::endl;

            std::cout << Indent(level) << "{" << std::endl;

            for (boost::property_tree::ptree::const_iterator pos = rootTree.begin(); pos != rootTree.end();) {
                std::cout << Indent(level + 1) << "\"" << pos->first << "\": ";

                PrintTree(pos->second, level + 1);
                ++pos;
                if (pos != rootTree.end()) {
                    std::cout << ",";
                }
                //std::cout << std::endl;
            }

            std::cout << Indent(level) << " }";
        }
        std::cout << std::endl;
    }
}// namespace AwsMock::Core