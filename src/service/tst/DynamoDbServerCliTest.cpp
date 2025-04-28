//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_COGNITO_SERVER_CLI_TEST_H
#define AWMOCK_COGNITO_SERVER_CLI_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/service/dynamodb/DynamoDbServer.h>
#include <awsmock/service/gateway/GatewayServer.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define ACCOUNT_ID "000000000000"
#define USER_NAME std::string("test-user")
#define USER_POOL_NAME std::string("test-user-pool")

namespace AwsMock::Service {

    /**
     * AwsMock DynamoDB integration test.
     */
    class DynamoDbServerCliTest : public testing::Test, public TestBase {

      protected:

        void SetUp() override {

            // General configuration
            StartGateway();
            _region = GetRegion();
            _endpoint = GetEndpoint();
        }

        void TearDown() override {
            for (const auto &table: _database.ListTables()) {
                const std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "delete-table", "--table-name", table.name, "--endpoint", _endpoint});
            }
            long count = _database.DeleteAllItems();
            log_debug << "DynamoDb items deleted: " << count;
            count = _database.DeleteAllTables();
            log_debug << "DynamoDb tables deleted: " << count;
        }

        static std::string WriteItemFile() {
            return Core::FileUtils::CreateTempFile("json", R"({"orgaNr":{"N":"1"}})");
        }

        static std::string WriteKeyFile() {
            return Core::FileUtils::CreateTempFile("json", R"({"orgaNr":{"N":"1"}})");
        }

        boost::thread _thread;
        std::string _endpoint, _region;
        boost::asio::io_context _ios{10};
        Core::Configuration &_configuration = Core::Configuration::instance();
        Database::DynamoDbDatabase &_database = Database::DynamoDbDatabase::instance();
        std::shared_ptr<GatewayServer> _gatewayServer;
    };

    TEST_F(DynamoDbServerCliTest, TableCreateTest) {

        // arrange

        // act
        std::string output = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "create-table", "--table-name", Core::StringUtils::GenerateRandomString(8), "--attribute-definitions", "AttributeName=orgaNr,AttributeType=N", "--key-schema", "AttributeName=orgaNr,KeyType=HASH", "--provisioned-throughput", "ReadCapacityUnits=1,WriteCapacityUnits=1", "--endpoint", _endpoint});
        const Database::Entity::DynamoDb::TableList tableList = _database.ListTables();

        // assert
        EXPECT_EQ(1, tableList.size());
    }

    TEST_F(DynamoDbServerCliTest, TableListTest) {

        // arrange
        std::string tableName = Core::StringUtils::GenerateRandomString(8);
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "create-table", "--table-name", tableName, "--attribute-definitions", "AttributeName=orgaNr,AttributeType=N", "--key-schema", "AttributeName=orgaNr,KeyType=HASH", "--provisioned-throughput", "ReadCapacityUnits=1,WriteCapacityUnits=1", "--endpoint", _endpoint});

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "list-tables", "--max-items", "10", "--endpoint", _endpoint});

        // assert
        EXPECT_FALSE(output2.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(output2, tableName));
    }

    TEST_F(DynamoDbServerCliTest, TableDescribeTest) {

        // arrange
        std::string tableName = Core::StringUtils::GenerateRandomString(8);
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "create-table", "--table-name", tableName, "--attribute-definitions", "AttributeName=orgaNr,AttributeType=N", "--key-schema", "AttributeName=orgaNr,KeyType=HASH", "--provisioned-throughput", "ReadCapacityUnits=1,WriteCapacityUnits=1", "--endpoint", _endpoint});
        const Database::Entity::DynamoDb::TableList tableList = _database.ListTables();
        EXPECT_EQ(1, tableList.size());

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "describe-table", "--table-name", tableName, "--endpoint", _endpoint});

        // assert
        EXPECT_FALSE(output2.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(output2, tableName));
    }

    TEST_F(DynamoDbServerCliTest, TableDeleteTest) {

        // arrange
        std::string tableName = Core::StringUtils::GenerateRandomString(8);
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "create-table", "--table-name", tableName, "--attribute-definitions", "AttributeName=orgaNr,AttributeType=N", "--key-schema", "AttributeName=orgaNr,KeyType=HASH", "--provisioned-throughput", "ReadCapacityUnits=1,WriteCapacityUnits=1", "--endpoint", _endpoint});
        const Database::Entity::DynamoDb::TableList tableList = _database.ListTables();
        EXPECT_EQ(1, tableList.size());

        // act
        const std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "delete-table", "--table-name", tableName, "--endpoint", _endpoint});

        // assert
        EXPECT_FALSE(output2.empty());
    }

    TEST_F(DynamoDbServerCliTest, ItemPutTest) {

        // arrange
        std::string tableName = Core::StringUtils::GenerateRandomString(8);
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "create-table", "--table-name", tableName, "--attribute-definitions", "AttributeName=orgaNr,AttributeType=N", "--key-schema", "AttributeName=orgaNr,KeyType=HASH", "--provisioned-throughput", "ReadCapacityUnits=1,WriteCapacityUnits=1", "--endpoint", _endpoint});
        Database::Entity::DynamoDb::TableList tableList = _database.ListTables();
        EXPECT_EQ(1, tableList.size());
        std::string itemFile = WriteItemFile();

        // act
        std::ofstream tmp("/tmp/debug.txt");
        tmp << AWS_CMD << " dynamodb put-item --table-name test-table1 --item file://" + itemFile + " --endpoint " + _endpoint << std::endl;
        tmp.close();
        std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "put-item", "--table-name", tableName, "--item", "file://" + itemFile, "--endpoint", _endpoint});

        // assert
        EXPECT_TRUE(output2.empty());
    }

    TEST_F(DynamoDbServerCliTest, ItemGetTest) {

        // arrange
        std::string tableName = Core::StringUtils::GenerateRandomString(8);
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "create-table", "--table-name", tableName, "--attribute-definitions", "AttributeName=orgaNr,AttributeType=N", "--key-schema", "AttributeName=orgaNr,KeyType=HASH", "--provisioned-throughput", "ReadCapacityUnits=1,WriteCapacityUnits=1", "--endpoint", _endpoint});
        const Database::Entity::DynamoDb::TableList tableList = _database.ListTables();
        EXPECT_EQ(1, tableList.size());
        const std::string itemFile = WriteItemFile();
        std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "put-item", "--table-name", tableName, "--item", "file://" + itemFile, "--endpoint", _endpoint});

        // act
        const std::string keyFile = WriteKeyFile();
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "get-item", "--table-name", tableName, "--key", "file://" + keyFile, "--endpoint", _endpoint});

        // assert
        EXPECT_FALSE(output3.empty());
    }

    TEST_F(DynamoDbServerCliTest, ItemScanTest) {

        // arrange
        std::string tableName = Core::StringUtils::GenerateRandomString(8);
        std::string output1 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "create-table", "--table-name", tableName, "--attribute-definitions", "AttributeName=orgaNr,AttributeType=N", "--key-schema", "AttributeName=orgaNr,KeyType=HASH", "--provisioned-throughput", "ReadCapacityUnits=1,WriteCapacityUnits=1", "--endpoint", _endpoint});
        const Database::Entity::DynamoDb::TableList tableList = _database.ListTables();
        EXPECT_EQ(1, tableList.size());
        const std::string itemFile = WriteItemFile();
        std::string output2 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "put-item", "--table-name", tableName, "--item", "file://" + itemFile, "--endpoint", _endpoint});

        // act
        std::string keyFile = WriteKeyFile();
        const std::string output3 = Core::TestUtils::SendCliCommand(AWS_CMD, {"dynamodb", "scan", "--table-name", tableName, "--endpoint", _endpoint});

        // assert
        EXPECT_FALSE(output3.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(output3, "orgaNr"));
    }

}// namespace AwsMock::Service

#endif// AWMOCK_COGNITO_SERVER_CLI_TEST_H