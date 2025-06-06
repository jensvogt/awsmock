//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_DYANMODB_SERVICE_TEST_H
#define AWMOCK_CORE_DYANMODB_SERVICE_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/service/dynamodb/DynamoDbService.h>

// Test includes
#include <awsmock/core/TestUtils.h>

#define REGION "eu-central-1"
#define TABLE_NAME "test-table"
#define OWNER "test-owner"

namespace AwsMock::Service {

    /**
     * @brief Test the dynamodb services of AwsMock.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class DynamoDbServiceTest : public testing::Test {

      protected:

        void SetUp() override {
        }

        void TearDown() override {
            const long count = _database.DeleteAllTables();
            log_debug << "Dynamodb tables deleted, count: " << count;
        }

        static std::map<std::string, std::string> CreateHeaders() {
            std::map<std::string, std::string> headers;
            headers["Accept-Encoding"] = "identity";
            headers["Content-Length"] = "244";
            headers["Content-Type"] = "application/x-amz-json-1.0";
            headers["Host"] = "localhost:14566";
            headers["User-Agent"] = "aws-cli/2.22.31 md/awscrt#0.23.4 ua/2.0 os/linux#6.8.0-51-generic md/arch#x86_64 lang/python#3.12.6 md/pyimpl#CPython cfg/retry-mode#standard md/installer#exe md/distrib#ubuntu.24 md/prompt#off md/command#dynamodb.create-table";
            headers["X-Amz-Date"] = "20250109T034230Z";
            headers["X-Amz-Target"] = "DynamoDB_20120810.CreateTable";
            return headers;
        }

        static std::string CreateBody() {
            return R"({"AttributeDefinitions":[{"AttributeName":"orgaNr","AttributeType":"N"}],"TableName":"test-table1","KeySchema":[{"AttributeName":"orgaNr","KeyType":"HASH"}],"ProvisionedThroughput":{"ReadCapacityUnits":1,"WriteCapacityUnits":1}})";
        }

        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(false);
        Database::DynamoDbDatabase _database = Database::DynamoDbDatabase();
        DynamoDbService _service;
    };

    /*
    TEST_F(DynamoDbServiceTest, TableCreateTest) {

        // arrange
        Dto::DynamoDb::ProvisionedThroughput provisionedThroughput;
        provisionedThroughput.readCapacityUnits = 1;
        provisionedThroughput.writeCapacityUnits = 1;
        std::map<std::string, std::string> attributes;
        attributes["AttributeName"] = "orgaNr";
        attributes["AttributeType"] = "N";
        std::map<std::string, std::string> keySchemas;
        keySchemas["AttributeName"] = "orgaNr";
        keySchemas["KeyType"] = "HASH";
        const Dto::DynamoDb::CreateTableRequest request = {.region = REGION, .tableName = TABLE_NAME, .attributes = attributes, .keySchemas = keySchemas, .provisionedThroughput = provisionedThroughput, .body = CreateBody(), .headers = CreateHeaders()};

        // act
        const Dto::DynamoDb::CreateTableResponse response = _service.CreateTable(request);

        // assert
        EXPECT_TRUE(response.tableName == TABLE_NAME);
    }
*/
}// namespace AwsMock::Service

#endif// AWMOCK_CORE_DYANMODB_SERVICE_TEST_H