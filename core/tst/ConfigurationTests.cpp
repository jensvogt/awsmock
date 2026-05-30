//
// Created by vogje01 on 02/06/2023.
//

// Boost includes
#include <boost/test/unit_test.hpp>

// Local includes
#include "awsmock/core/TestUtils.h"


#include <awsmock/core/config/Configuration.h>

namespace AwsMock::Core {

    BOOST_AUTO_TEST_CASE(ConstructorTest) {

        // arrange

        // act

        // assert
        BOOST_CHECK_EQUAL(Configuration::instance().filePath(), TMP_CONFIGURATION_FILE);
        BOOST_CHECK_EQUAL(Configuration::instance().get<std::string>("awsmock.logging.level").empty(), false);
    }

    BOOST_AUTO_TEST_CASE(EnvironmentTest) {

        // arrange
#ifdef WIN32
        _putenv("AWSMOCK_LOG_LEVEL=error");
#else
        setenv("AWSMOCK_LOG_LEVEL", "error", true);
#endif
        Configuration &configuration = Configuration::instance();
        configuration.setFilePath(TMP_CONFIGURATION_FILE);

        // act

        // assert
        BOOST_CHECK_EQUAL(configuration.get<std::string>("awsmock.logging.level"), "error");
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
        Configuration &configuration = Configuration::instance();
        configuration.setFilePath(jsonFile);

        // act
        const auto region = configuration.get<std::string>("awsmock.region");
        const auto keyId = configuration.get<std::string>("awsmock.access.key-id");

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
        Configuration &cfg = Configuration::instance();
        cfg.load(jsonFile);

        // act
        const std::vector<std::string> directories = cfg.getArray<std::string>("awsmock.modules.transfer.directories");

        // assert
        BOOST_CHECK_EQUAL(2, directories.size());
        BOOST_CHECK_EQUAL(directories[0], "/incoming/mix");
        BOOST_CHECK_EQUAL(directories[1], "/feedback");
    }

}// namespace AwsMock::Core
