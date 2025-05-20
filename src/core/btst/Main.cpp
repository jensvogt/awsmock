//
// Created by vogje01 on 5/20/25.
//

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE CoreTests

#include "AwsUtilsTests.cpp"
#include "ConfigurationTests.cpp"
#include "CryptoUtilsTest.cpp"
#include "DirUtilsTests.cpp"
#include "FileUtilsTests.cpp"

// initialization function:
bool init_unit_test() {
    AwsMock::Core::LogStream::Initialize();
    AwsMock::Core::LogStream::SetSeverity("error");
    AwsMock::Core::TestUtils::CreateTestConfigurationFile(false);
    return true;
}

// entry point:
int main(const int argc, char *argv[]) {
    return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}