//
// Created by vogje01 on 5/20/25.
//

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE CoreTests

#include "AwsUtilsTests.cpp"
#include "ConfigurationTests.cpp"
#include "CronUtilsTests.cpp"
#include "CryptoUtilsTest.cpp"
#include "DateTimeUtilsTests.cpp"
#include "DirUtilsTests.cpp"
#include "FileUtilsTests.cpp"
#include "HttpUtilsTests.cpp"
#include "JwtUtilsTests.cpp"
#include "RandomUtilsTests.cpp"
#include "StringUtilsTests.cpp"
#include "SystemUtilsTests.cpp"
#include "TarUtilsTests.cpp"
#include "XmlUtilsTests.cpp"

// initialization function:
bool init_unit_test() {
    AwsMock::Core::LogStream::Initialize();
    AwsMock::Core::LogStream::RemoveConsoleLogs();
    AwsMock::Core::TestUtils::CreateTestConfigurationFile(false);
    return true;
}

// entry point:
int main(const int argc, char *argv[]) {
    return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}