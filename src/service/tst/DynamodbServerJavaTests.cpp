//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_DYNAMODB_SERVER_JAVA_TEST_H
#define AWMOCK_DYNAMODB_SERVER_JAVA_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/service/dynamodb/DynamoDbServer.h>
#include <awsmock/service/dynamodb/DynamoDbService.h>
#include <awsmock/service/gateway/GatewayServer.h>

#define REGION "eu-central-1"
#define TEST_TABLE "test-table"
#define KEY "testfile.json"
#define OWNER "test-owner"
#define TEST_PORT 10100

namespace AwsMock::Service {

    class DynamoDbServerJavaTest : public ::testing::Test {

      protected:

        void SetUp() override {

            // General configuration
            _region = _configuration.GetValueString("awsmock.region");

            // Define endpoint. This is the endpoint of the SQS server, not the gateway
            _configuration.SetValueInt("awsmock.gateway.http.port", TEST_PORT + 1);
            _configuration.SetValueString("awsmock.gateway.http.host", "localhost");

            // Base URL
            _baseUrl = "/api/dynamodb/";

            // Start HTTP manager
            _gatewayServer = std::make_shared<GatewayServer>(_ios);
            _thread = boost::thread([&]() {
                _ios.run();
            });
        }

        void TearDown() override {
            _dynamoDbService.DeleteAllTables();
        }

        static Core::HttpSocketResponse SendGetCommand(const std::string &url, const std::string &payload) {
            std::map<std::string, std::string> headers;
            headers[to_string(http::field::content_type)] = "application/json";
            Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::get, "localhost", TEST_PORT, url, payload, headers);
            log_debug << "Status: " << response.statusCode << " body: " << response.body;
            return response;
        }

        static Core::HttpSocketResponse SendPostCommand(const std::string &url, const std::string &payload) {
            std::map<std::string, std::string> headers;
            headers[to_string(http::field::content_type)] = "application/json";
            Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::post, "localhost", TEST_PORT, url, payload, headers);
            log_debug << "Status: " << response.statusCode << " body: " << response.body;
            return response;
        }

        static Core::HttpSocketResponse SendDeleteCommand(const std::string &url, const std::string &payload) {
            std::map<std::string, std::string> headers;
            headers[to_string(http::field::content_type)] = "application/json";
            Core::HttpSocketResponse response = Core::HttpSocket::SendJson(http::verb::delete_, "localhost", TEST_PORT, url, payload, headers);
            log_debug << "Status: " << response.statusCode << " body: " << response.body;
            return response;
        }

        boost::thread _thread;
        std::string _endpoint, _baseUrl, _region;
        boost::asio::io_service _ios{10};
        Core::Configuration &_configuration = Core::Configuration::instance();
        Database::DynamoDbDatabase &_database = Database::DynamoDbDatabase::instance();
        std::shared_ptr<GatewayServer> _gatewayServer;
        DynamoDbService _dynamoDbService;
    };

    TEST_F(DynamoDbServerJavaTest, TableCreateTest) {

        // arrange

        // act
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createTable?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});
        Database::Entity::DynamoDb::TableList tableList = _database.ListTables();

        // assert
        EXPECT_TRUE(result.statusCode == http::status::ok);
        EXPECT_EQ(1, tableList.size());
    }

    TEST_F(DynamoDbServerJavaTest, TableListTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createTable?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::DynamoDb::TableList tableList = _database.ListTables();
        EXPECT_EQ(1, tableList.size());

        // act
        Core::HttpSocketResponse listResult = SendGetCommand(_baseUrl + "listTables?limit=10", {});

        // assert
        EXPECT_TRUE(listResult.statusCode == http::status::ok);
    }

    TEST_F(DynamoDbServerJavaTest, TableDescribeTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createTable?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::DynamoDb::TableList tableList = _database.ListTables();
        EXPECT_EQ(1, tableList.size());

        // act
        Core::HttpSocketResponse describeResult = SendGetCommand(_baseUrl + "describeTable?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});

        // assert
        EXPECT_TRUE(describeResult.statusCode == http::status::ok);
    }

    TEST_F(DynamoDbServerJavaTest, TableDeleteTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createTable?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::DynamoDb::TableList tableList = _database.ListTables();
        EXPECT_EQ(1, tableList.size());

        // act
        Core::HttpSocketResponse deleteResult = SendDeleteCommand(_baseUrl + "deleteTable?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});
        tableList = _database.ListTables();

        // assert
        EXPECT_TRUE(deleteResult.statusCode == http::status::ok);
        EXPECT_EQ(0, tableList.size());
    }

    TEST_F(DynamoDbServerJavaTest, ItemPutTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createTable?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::DynamoDb::TableList tableList = _database.ListTables();
        EXPECT_EQ(1, tableList.size());

        // act
        Core::HttpSocketResponse putItemResult = SendPostCommand(_baseUrl + "putItem?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});
        long itemCount = _database.CountItems();

        // assert
        EXPECT_TRUE(putItemResult.statusCode == http::status::ok);
        EXPECT_EQ(1, itemCount);
    }

    TEST_F(DynamoDbServerJavaTest, ItemGetTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createTable?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::DynamoDb::TableList tableList = _database.ListTables();
        EXPECT_EQ(1, tableList.size());
        Core::HttpSocketResponse putItemResult = SendPostCommand(_baseUrl + "putItem?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});
        long itemCount = _database.CountItems();
        EXPECT_EQ(1, itemCount);

        // act
        Core::HttpSocketResponse getItemResult = SendGetCommand(_baseUrl + "getItem?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), R"({"orgaNr":{"N":"1"}})");

        // assert
        EXPECT_TRUE(putItemResult.statusCode == http::status::ok);
        EXPECT_EQ(1, itemCount);
    }

    TEST_F(DynamoDbServerJavaTest, ItemScanTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createTable?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::DynamoDb::TableList tableList = _database.ListTables();
        EXPECT_EQ(1, tableList.size());
        Core::HttpSocketResponse putItemResult = SendPostCommand(_baseUrl + "putItem?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});
        long itemCount = _database.CountItems();
        EXPECT_EQ(1, itemCount);

        // act
        Core::HttpSocketResponse scanResult = SendGetCommand(_baseUrl + "scan?tableName=" + Core::StringUtils::UrlEncode(TEST_TABLE), {});

        // assert
        EXPECT_TRUE(scanResult.statusCode == http::status::ok);
        EXPECT_EQ(1, itemCount);
    }

}// namespace AwsMock::Service

#endif// AWMOCK_DYNAMODB_SERVER_JAVA_TEST_H