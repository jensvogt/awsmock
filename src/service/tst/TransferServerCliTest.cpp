//
// Created by vogje01 on 02/06/2023.
//

#include "TestBase.h"
#ifndef AWMOCK_SERVICE_TRANSFER_SERVER_CLI_TEST_H
#define AWMOCK_SERVICE_TRANSFER_SERVER_CLI_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/FileUtils.h>
#include <awsmock/service/gateway/GatewayServer.h>
#include <awsmock/service/s3/S3Server.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define TEST_BUCKET_NAME std::string("test-bucket")
#define TEST_BUCKET std::string("s3://test-bucket")
#define S3_ACCOUNT_ID "000000000000"

namespace AwsMock::Service {

    /**
     * @brief Test the Transfer Server command line interface of AwsMock,
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TransferServerCliTest : public ::testing::Test, public TestBase {

      protected:

        void SetUp() override {
            StartGateway();
        }

        void TearDown() override {
            _database.DeleteAllTransfers();
        }
        Database::TransferDatabase &_database = Database::TransferDatabase::instance();
    };

    TEST_F(TransferServerCliTest, TransferServerEmptyListTest) {

        // arrange

        // act
        std::string output = Core::TestUtils::SendCliCommand("aws", {"transfer", "list-servers", "--endpoint", GetEndpoint()});
        const Database::Entity::Transfer::TransferList transferList = _database.ListServers();

        // assert
        EXPECT_EQ(0, transferList.size());
    }

    TEST_F(TransferServerCliTest, TransferServerListTest) {

        // arrange
        std::string output1 = Core::TestUtils::SendCliCommand("aws", {"transfer", "create-server", "--protocols", "ftp", "--endpoint", GetEndpoint()});
        Database::Entity::Transfer::TransferList transferList = _database.ListServers();
        EXPECT_EQ(1, transferList.size());

        // act
        std::string output3 = Core::TestUtils::SendCliCommand("aws", {"transfer", "list-servers", "--endpoint", GetEndpoint()});
        transferList = _database.ListServers();

        // assert
        EXPECT_EQ(1, transferList.size());
    }

    TEST_F(TransferServerCliTest, TransferServerCreateTest) {

        // arrange

        // act
        const std::string output = Core::TestUtils::SendCliCommand("aws", {"transfer", "create-server", "--protocols", "ftp", "--endpoint", GetEndpoint()});
        const Database::Entity::Transfer::TransferList transferList = _database.ListServers();

        // assert
        EXPECT_EQ(1, transferList.size());
        EXPECT_FALSE(transferList.front().serverId.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(output, "ServerId"));
    }

    TEST_F(TransferServerCliTest, TransferServerDeleteTest) {

        // arrange
        const std::string output1 = Core::TestUtils::SendCliCommand("aws", {"transfer", "create-server", "--protocols", "ftp", "--endpoint", GetEndpoint()});
        Database::Entity::Transfer::TransferList transferList = _database.ListServers();
        const std::string serverId = transferList.front().serverId;

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand("aws", {"transfer", "delete-server", "--server-id", serverId, "--endpoint", GetEndpoint()});
        transferList = _database.ListServers();

        // assert
        EXPECT_EQ(0, transferList.size());
    }

    TEST_F(TransferServerCliTest, TransferServerStartTest) {

        // arrange
        const std::string output1 = Core::TestUtils::SendCliCommand("aws", {"transfer", "create-server", "--protocols", "ftp", "--endpoint", GetEndpoint()});
        Database::Entity::Transfer::TransferList transferList = _database.ListServers();
        const std::string serverId = transferList.front().serverId;

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand("aws", {"transfer", "start-server", "--server-id", serverId, "--endpoint", GetEndpoint()});
        transferList = _database.ListServers();

        // assert
        EXPECT_TRUE(transferList.front().state == Database::Entity::Transfer::ServerState::ONLINE);
    }

    TEST_F(TransferServerCliTest, TransferServerStopTest) {

        // arrange
        const std::string output1 = Core::TestUtils::SendCliCommand("aws", {" transfer", "create-server", "--protocols", "ftp", "--endpoint", GetEndpoint()});
        Database::Entity::Transfer::TransferList transferList = _database.ListServers();
        const std::string serverId = transferList.front().serverId;
        const std::string output2 = Core::TestUtils::SendCliCommand("aws", {" transfer", "start-server", "--server-id", serverId, "--endpoint", GetEndpoint()});
        transferList = _database.ListServers();
        EXPECT_FALSE(transferList.empty());

        // act
        const std::string output3 = Core::TestUtils::SendCliCommand("aws", {"transfer", "stop-server", "--server-id", serverId, "--endpoint", GetEndpoint()});
        transferList = _database.ListServers();

        // assert
        EXPECT_TRUE(transferList.front().state == Database::Entity::Transfer::ServerState::OFFLINE);
    }

}// namespace AwsMock::Service

#endif// AWMOCK_SERVICE_TRANSFER_SERVER_CLI_TEST_H