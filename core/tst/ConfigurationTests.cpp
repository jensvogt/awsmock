//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWS_MOCK_CORE_CONFIGURATION_TEST_H
#define AWS_MOCK_CORE_CONFIGURATION_TEST_H

// Local includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/core/config/Configuration.h>

namespace AwsMock::Core {

    BOOST_AUTO_TEST_CASE(EmptyFilenameTest) {
        // arrange

        // act
        BOOST_CHECK_THROW({
                     try {
                        auto configuration = Configuration("");
                     } catch (const CoreException &e) {
                         BOOST_CHECK_EQUAL("Empty configuration filename", e.message());
                         throw;
                     } }, CoreException);

        // assert
    }

    BOOST_AUTO_TEST_CASE(NonexistentFilenameTest) {
        // arrange

        // act
        BOOST_CHECK_THROW({
                     try {
                        auto configuration = Configuration("FooBarBaz.json");
                     } catch (const CoreException &e) {
                         BOOST_CHECK_EQUAL("Configuration file 'FooBarBaz.json' does not exist. Will use defaults.", e.message());
                         throw;
                     } }, CoreException);

        // assert
    }

    BOOST_AUTO_TEST_CASE(ConstructorTest) {

        // arrange

        // act

        // assert
        BOOST_CHECK_EQUAL(Configuration::instance().GetFilename(), TMP_CONFIGURATION_FILE);
        BOOST_CHECK_EQUAL(Configuration::instance().GetValue<std::string>("awsmock.logging.level").empty(), false);
    }

    BOOST_AUTO_TEST_CASE(EnvironmentTest) {

        // arrange
#ifdef WIN32
        _putenv("AWSMOCK_LOG_LEVEL=error");
#else
        setenv("AWSMOCK_LOG_LEVEL", "error", true);
#endif
        Configuration configuration;
        configuration.SetFilename(TMP_CONFIGURATION_FILE);

        // act

        // assert
        BOOST_CHECK_EQUAL(configuration.GetValue<std::string>("awsmock.logging.level"), "error");
    }

    BOOST_AUTO_TEST_CASE(JsonConfigurationTest) {

        // arrange
        const std::string jsonString = "{\n"
                                       "  \"awsmock\":{\n"
                                       "    \"region\": \"eu-central-1\",\n"
                                       "    \"user\": \"none\",\n"
                                       "    \"access\": {\n"
                                       "      \"key-id\": \"none\",\n"
                                       "      \"secret-access-key\": \"none\",\n"
                                       "      \"account-id\": \"000000000000\"\n"
                                       "    },\n"
                                       "    \"logging\": {\n"
                                       "      \"level\": \"debug\"\n"
                                       "    }\n"
                                       "  }\n"
                                       "}";
        const std::string jsonFile = FileUtils::CreateTempFile("json", jsonString);
        Configuration configuration;
        configuration.SetFilename(jsonFile);

        // act
        const auto region = configuration.GetValue<std::string>("awsmock.region");
        const auto keyId = configuration.GetValue<std::string>("awsmock.access.key-id");

        // assert
        BOOST_CHECK_EQUAL(region, "eu-central-1");
        BOOST_CHECK_EQUAL(keyId, "none");
    }

    BOOST_AUTO_TEST_CASE(JsonConfigurationArrayTest) {

        // arrange
        const std::string jsonString = "{\n"
                                       "  \"awsmock\":{\n"
                                       "    \"region\": \"eu-central-1\",\n"
                                       "    \"user\": \"none\",\n"
                                       "    \"access\": {\n"
                                       "      \"key-id\": \"none\",\n"
                                       "      \"secret-access-key\": \"none\",\n"
                                       "      \"account-id\": \"000000000000\"\n"
                                       "    },\n"
                                       "    \"modules\": {\n"
                                       "      \"transfer\": {\n"
                                       "        \"directories\": [\n"
                                       "          \"/incoming/mix\",\n"
                                       "          \"/feedback\"\n"
                                       "        ]\n"
                                       "      }\n"
                                       "    },\n"
                                       "    \"logging\": {\n"
                                       "      \"level\": \"debug\"\n"
                                       "    }\n"
                                       "  }\n"
                                       "}\n";
        const std::string jsonFile = FileUtils::CreateTempFile("json", jsonString);
        Configuration configuration;
        configuration.SetFilename(jsonFile);

        // act
        const std::vector<std::string> directories = configuration.GetValueArray<std::string>("awsmock.modules.transfer.directories");

        // assert
        BOOST_CHECK_EQUAL(2, directories.size());
    }

}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_CONFIGURATION_TEST_H
