//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWS_MOCK_CORE_CONFIGURATION_TEST_H
#define AWS_MOCK_CORE_CONFIGURATION_TEST_H

// GTest includes
#include <gtest/gtest.h>

// Local includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/core/config/Configuration.h>

namespace AwsMock::Core {
    class ConfigurationTest : public testing::Test {
    };

    TEST_F(ConfigurationTest, EmptyFilenameTest) {
        // arrange

        // act
        EXPECT_THROW({
                     try {
                     auto configuration = Configuration("");
                     } catch (const CoreException &e) {
                     EXPECT_STREQ("Empty filename", e.message().c_str());
                     throw;
                     } }, CoreException);

        // assert
    }

    TEST_F(ConfigurationTest, ConstructorTest) {

        // arrange
        const Configuration configuration = Configuration::instance();
        Configuration::instance().SetFilename(TMP_PROPERTIES_FILE);

        // act

        // assert
        EXPECT_STREQ(Configuration::instance().GetFilename().c_str(), TMP_PROPERTIES_FILE);
        EXPECT_FALSE(Configuration::instance().GetValueString("awsmock.logging.level").empty());
    }

    TEST_F(ConfigurationTest, EnvironmentTest) {

        // arrange
#ifdef WIN32
        _putenv("AWSMOCK_LOG_LEVEL=error");
#else
        setenv("AWSMOCK_LOG_LEVEL", "error", true);
#endif
        const Configuration configuration = Configuration::instance();
        Configuration::instance().SetFilename(TMP_PROPERTIES_FILE);

        // act

        // assert
        EXPECT_STREQ(configuration.GetValueString("awsmock.logging.level").c_str(), "debug");
    }

    TEST_F(ConfigurationTest, YamlConfigurationTest) {
        // arrange
        const std::string yamlString = "awsmock:\n"
                                       "  region: eu-central-1\n"
                                       "  user: none\n"
                                       "  access: \n"
                                       "    key-id: none\n"
                                       "    secret-access-key: none\n"
                                       "    account-id: 000000000000\n";
        const std::string yamlFile = FileUtils::CreateTempFile("yaml", yamlString);
        Configuration::instance().SetFilename(yamlFile);

        // act
        const std::string region = Configuration::instance().GetValueString("awsmock.region");
        const std::string keyId = Configuration::instance().GetValueString("awsmock.access.key-id");

        // assert
        EXPECT_TRUE(region == "eu-central-1");
        EXPECT_TRUE(keyId == "none");
    }

    TEST_F(ConfigurationTest, YamlConfigurationArrayTest) {
        // arrange
        const std::string yamlString = "awsmock:\n"
                                       "  region: eu-central-1\n"
                                       "  user: none\n"
                                       "  access: \n"
                                       "    key-id: none\n"
                                       "    secret-access-key: none\n"
                                       "    account-id: 000000000000\n"
                                       "  modules: \n"
                                       "    transfer: \n"
                                       "      directories:\n"
                                       "        - /incoming/mix\n"
                                       "        - /feedback\n";
        const std::string yamlFile = FileUtils::CreateTempFile("yaml", yamlString);
        Configuration::instance().SetFilename(yamlFile);

        // act
        const std::vector<std::string> directories = Configuration::instance().GetValueStringArray("awsmock.modules.transfer.directories");

        // assert
        EXPECT_EQ(2, directories.size());
    }

}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_CONFIGURATION_TEST_H
