//
// Created by vogje01 on 21/10/2023.
//

#ifndef AWMOCK_SERVICE_SSM_CLI_INTEGRATION_TEST_H
#define AWMOCK_SERVICE_SSM_CLI_INTEGRATION_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/SSMDatabase.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/ssm/SSMServer.h>

#define TEST_PARAMETER_NAME std::string("test-name")
#define TEST_PARAMETER_VALUE std::string("test-value")

namespace AwsMock::Service {

    class SSMServerCliTest : public ::testing::Test, public TestBase {

      protected:

        void SetUp() override {

            // General configuration
            StartGateway();
            _region = GetRegion();
            _endpoint = GetEndpoint();
        }

        void TearDown() override {
            _ssmDatabase.DeleteAllParameters();
        }

        std::string _endpoint, _region;
        Database::SSMDatabase &_ssmDatabase = Database::SSMDatabase::instance();
    };

    TEST_F(SSMServerCliTest, ParameterPutTest) {

        // arrange

        // act
        const std::string output = Core::TestUtils::SendCliCommand("aws", {"ssm", "put-parameter", "--name", TEST_PARAMETER_NAME, "--value", TEST_PARAMETER_VALUE, "--endpoint", _endpoint});
        const Database::Entity::SSM::ParameterList parameterList = _ssmDatabase.ListParameters();

        // assert
        EXPECT_EQ(1, parameterList.size());
        EXPECT_TRUE(Core::StringUtils::Contains(output, "Version"));
    }

    TEST_F(SSMServerCliTest, ParameterGetTest) {

        // arrange
        const std::string output1 = Core::TestUtils::SendCliCommand("aws", {"ssm", "put-parameter", "--name", TEST_PARAMETER_NAME, "--value", TEST_PARAMETER_VALUE, "--endpoint", _endpoint});
        const Database::Entity::SSM::ParameterList parameterList = _ssmDatabase.ListParameters();
        EXPECT_EQ(1, parameterList.size());

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand("aws", {"ssm", "get-parameter", "--name", TEST_PARAMETER_NAME, "--endpoint", _endpoint});

        // assert
        EXPECT_EQ(1, parameterList.size());
        EXPECT_TRUE(Core::StringUtils::Contains(output2, TEST_PARAMETER_NAME));
    }

    TEST_F(SSMServerCliTest, ParameterDescribeTest) {

        // arrange
        const std::string output1 = Core::TestUtils::SendCliCommand("aws", {"ssm", "put-parameter", "--name", TEST_PARAMETER_NAME, "--value", TEST_PARAMETER_VALUE, "--endpoint", _endpoint});
        Database::Entity::SSM::ParameterList parameterList = _ssmDatabase.ListParameters();
        EXPECT_EQ(1, parameterList.size());

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand("aws", {"ssm", "describe", "--parameters", "--endpoint", _endpoint});

        // assert
        EXPECT_TRUE(Core::StringUtils::Contains(output2, TEST_PARAMETER_NAME));
    }

    TEST_F(SSMServerCliTest, ParameterDeleteTest) {
        // arrange
        const std::string output1 = Core::TestUtils::SendCliCommand("aws", {"ssm", "put-parameter", "--name", TEST_PARAMETER_NAME, "--value", TEST_PARAMETER_VALUE, "--endpoint", _endpoint});
        Database::Entity::SSM::ParameterList parameterList = _ssmDatabase.ListParameters();
        EXPECT_EQ(1, parameterList.size());

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand("aws", {"ssm", "delete-parameter", "--name", TEST_PARAMETER_NAME, "--endpoint", _endpoint});
        parameterList = _ssmDatabase.ListParameters();

        // assert
        EXPECT_EQ(0, parameterList.size());
    }

}// namespace AwsMock::Service

#endif// AWMOCK_SERVICE_SSM_CLI_INTEGRATION_TEST_H