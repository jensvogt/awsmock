//
// Created by vogje01 on 22/08/2022.
//

#ifndef AWSMOCK_CORE_XML_UTILS_H
#define AWSMOCK_CORE_XML_UTILS_H

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN
#include <boost/asio.hpp>
#include <windows.h>
#endif


// Standard C++ includes
#include <string>

// Boost includes
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

// AwsMOck includes
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    /**
     * @brief XML utilities.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class XmlUtils {

      public:

        /**
         * @brief Returns the XML string for the document.
         *
         * @param rootTree property tree
         * @return XML string
         */
        static std::string ToXmlString(const boost::property_tree::ptree &rootTree);

        /**
         * @brief Reads an XML string into a boost property tree
         *
         * @param xmlString XML string
         * @param rootTree boost property tree
         */
        static void ReadXml(const std::string &xmlString, boost::property_tree::ptree *rootTree);

        /**
         * @brief Reads an XML string into a boost property tree
         *
         * @param rootTree boost property tree
         * @param level indentation spaces
         */
        static void PrintTree(const boost::property_tree::ptree &rootTree, int level = 2);

      private:

        /**
         * @brief Indentation
         *
         * @param level indentation spaces
         */
        static std::string Indent(int level = 2);
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_XML_UTILS_H
