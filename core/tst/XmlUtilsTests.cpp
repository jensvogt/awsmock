//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_XML_UTILS_TEST_H
#define AWMOCK_CORE_XML_UTILS_TEST_H

// Local includes
#include <awsmock/core/XmlUtils.h>

namespace AwsMock::Core {

    BOOST_AUTO_TEST_CASE(CreateTextNodeTest) {

        // arrange
        boost::property_tree::ptree root;
        root.add("Root", "");
        root.add("Root.Test", "TestValue");
        const std::string xmlString = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Root><Test>TestValue</Test></Root>";

        // act
        const std::string result = XmlUtils::ToXmlString(root);

        // assert
        BOOST_CHECK_EQUAL(Core::StringUtils::StripLineEndings(result), xmlString);
    }


    BOOST_AUTO_TEST_CASE(CreateTextArrayTest) {

        // arrange
        boost::property_tree::ptree root;
        root.add("Root", "");
        root.add("Root.Test", "TestValue");
        root.add("Root.Test.TextValue", "Value1");
        root.add("Root.Test.TextValue", "Value2");
        root.add("Root.Test.TextValue", "Value3");
        const std::string xmlString = "<?xml version=\"1.0\" encoding=\"utf-8\"?><Root><Test>TestValue<TextValue>Value1</TextValue><TextValue>Value2</TextValue><TextValue>Value3</TextValue></Test></Root>";

        // act
        const std::string result = XmlUtils::ToXmlString(root);

        // assert
        BOOST_CHECK_EQUAL(Core::StringUtils::StripLineEndings(result), xmlString);
    }

}// namespace AwsMock::Core

#endif// AWMOCK_CORE_XML_UTILS_TEST_H