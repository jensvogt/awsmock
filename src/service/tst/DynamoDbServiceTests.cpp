//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_SERVICE_DYANMODB_SERVICE_TEST_H
#define AWMOCK_SERVICE_DYANMODB_SERVICE_TEST_H

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

namespace Awsmock::Service {

    class DynamoDbServiceTest : public testing::Test {

      protected:

        void SetUp() override {
        }

        void TearDown() override {
            const long count = _service.DeleteAllTables();
            log_debug << "DynamoDB tables deleted, count: " << count;
        }

        static Dto::DynamoDb::CreateTableRequest CreateDefaultTableRequest() {
            Dto::DynamoDb::CreateTableRequest request;
            request.region = REGION;
            request.tableName = TABLE_NAME;

            Dto::DynamoDb::AttributeDefinition attr1;
            attr1.attributeName = "datenlieferantId";
            attr1.attributeType = "S";

            Dto::DynamoDb::AttributeDefinition attr2;
            attr2.attributeName = "ean";
            attr2.attributeType = "S";

            Dto::DynamoDb::AttributeDefinition attr3;
            attr3.attributeName = "webSite";
            attr3.attributeType = "S";

            Dto::DynamoDb::AttributeDefinition attr4;
            attr4.attributeName = "town";
            attr4.attributeType = "S";

            Dto::DynamoDb::AttributeDefinition attr5;
            attr5.attributeName = "street";
            attr5.attributeType = "S";

            Dto::DynamoDb::AttributeDefinition attr6;
            attr6.attributeName = "streetNo";
            attr6.attributeType = "N";

            request.attributes = {attr1, attr2, attr3, attr4, attr5, attr6};

            Dto::DynamoDb::KeySchema key1;
            key1.attributeName = "datenlieferantId";
            key1.keyType = "HASH";

            Dto::DynamoDb::KeySchema key2;
            key2.attributeName = "ean";
            key2.keyType = "RANGE";

            request.keySchema = {key1, key2};
            return request;
        }

        static Dto::DynamoDb::AttributeValue MakeString(const std::string &value) {
            Dto::DynamoDb::AttributeValue av;
            av.type = "S";
            av.stringValue = value;
            return av;
        }

        static Dto::DynamoDb::AttributeValue MakeNumber(int value) {
            Dto::DynamoDb::AttributeValue av;
            av.type = "N";
            av.numberValue = std::to_string(value);
            return av;
        }

        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(false);
        Database::DynamoDbDatabase &_database = Database::DynamoDbDatabase::instance();
        DynamoDbService _service;
    };

    TEST_F(DynamoDbServiceTest, TableCreateTest) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest request = CreateDefaultTableRequest();

        // act
        const Dto::DynamoDb::CreateTableResponse response = _service.CreateTable(request);

        // assert
        EXPECT_FALSE(response.tableArn.empty());
        EXPECT_EQ(TABLE_NAME, response.tableName);
        EXPECT_FALSE(response.keySchemas.empty());
        EXPECT_FALSE(response.attributeDefinitions.empty());
    }

    TEST_F(DynamoDbServiceTest, ListTablesTest) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::ListTableRequest listRequest;
        listRequest.region = REGION;

        // act
        const Dto::DynamoDb::ListTableResponse listResponse = _service.ListTables(listRequest);

        // assert
        EXPECT_FALSE(listResponse.tableNames.empty());
        EXPECT_EQ(1, listResponse.tableNames.size());
        EXPECT_EQ(TABLE_NAME, listResponse.tableNames.at(0));
    }

    TEST_F(DynamoDbServiceTest, DescribeTableTest) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::DescribeTableRequest descRequest;
        descRequest.region = REGION;
        descRequest.tableName = TABLE_NAME;

        // act
        const Dto::DynamoDb::DescribeTableResponse descResponse = _service.DescribeTable(descRequest);

        // assert
        EXPECT_EQ(TABLE_NAME, descResponse.tableName);
        EXPECT_FALSE(descResponse.tableArn.empty());
        EXPECT_FALSE(descResponse.keySchema.empty());
        EXPECT_FALSE(descResponse.attributeDefinitions.empty());
    }

    TEST_F(DynamoDbServiceTest, PutItemTest) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::PutItemRequest putRequest;
        putRequest.region = REGION;
        putRequest.tableName = TABLE_NAME;
        putRequest.attributes["datenlieferantId"] = MakeString("DLI333");
        putRequest.attributes["ean"] = MakeString("ABC");
        putRequest.attributes["webSite"] = MakeString("http://DLI333/public");
        putRequest.attributes["town"] = MakeString("Hamburg");
        putRequest.attributes["street"] = MakeString("Elbchaussee");
        putRequest.attributes["streetNo"] = MakeNumber(265);

        // act
        const Dto::DynamoDb::PutItemResponse putResponse = _service.PutItem(putRequest);

        // assert
        EXPECT_EQ(TABLE_NAME, putResponse.item.tableName);
        EXPECT_FALSE(putResponse.item.attributes.empty());
        EXPECT_EQ(6, putResponse.item.attributes.size());
        EXPECT_EQ("DLI333", putResponse.item.attributes.at("datenlieferantId").stringValue);
        EXPECT_EQ("ABC", putResponse.item.attributes.at("ean").stringValue);
    }

    TEST_F(DynamoDbServiceTest, GetItemTest) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::PutItemRequest putRequest;
        putRequest.region = REGION;
        putRequest.tableName = TABLE_NAME;
        putRequest.attributes["datenlieferantId"] = MakeString("DLI333");
        putRequest.attributes["ean"] = MakeString("ABC");
        putRequest.attributes["webSite"] = MakeString("http://DLI333/public");
        putRequest.attributes["town"] = MakeString("Hamburg");
        putRequest.attributes["street"] = MakeString("Elbchaussee");
        putRequest.attributes["streetNo"] = MakeNumber(265);
        _service.PutItem(putRequest);

        Dto::DynamoDb::GetItemRequest getRequest;
        getRequest.region = REGION;
        getRequest.tableName = TABLE_NAME;
        getRequest.keys["datenlieferantId"] = MakeString("DLI333");
        getRequest.keys["ean"] = MakeString("ABC");

        // act
        const Dto::DynamoDb::GetItemResponse getResponse = _service.GetItem(getRequest);

        // assert
        EXPECT_FALSE(getResponse.attributes.empty());
        EXPECT_EQ("DLI333", getResponse.attributes.at("datenlieferantId").stringValue);
        EXPECT_EQ("ABC", getResponse.attributes.at("ean").stringValue);
    }

    TEST_F(DynamoDbServiceTest, ScanTest) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::PutItemRequest putRequest;
        putRequest.region = REGION;
        putRequest.tableName = TABLE_NAME;
        putRequest.attributes["datenlieferantId"] = MakeString("DLI333");
        putRequest.attributes["ean"] = MakeString("ABC");
        _service.PutItem(putRequest);

        Dto::DynamoDb::ScanRequest scanRequest;
        scanRequest.region = REGION;
        scanRequest.tableName = TABLE_NAME;

        // act
        const Dto::DynamoDb::ScanResponse scanResponse = _service.Scan(scanRequest);

        // assert
        EXPECT_FALSE(scanResponse.items.empty());
        EXPECT_EQ(1, scanResponse.count);
    }

    TEST_F(DynamoDbServiceTest, QueryItemTest1) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::PutItemRequest putRequest;
        putRequest.region = REGION;
        putRequest.tableName = TABLE_NAME;
        putRequest.attributes["datenlieferantId"] = MakeString("DLI2361");
        _service.PutItem(putRequest);

        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = REGION;
        queryRequest.tableName = TABLE_NAME;
        queryRequest.limit = 10;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = MakeString("DLI2361");

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _service.Query(queryRequest);

        // assert
        EXPECT_FALSE(queryResponse.tableName.empty());
        EXPECT_FALSE(queryResponse.items.empty());
    }

    TEST_F(DynamoDbServiceTest, QueryItemTest2) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::PutItemRequest putRequest1;
        putRequest1.region = REGION;
        putRequest1.tableName = TABLE_NAME;
        putRequest1.attributes["datenlieferantId"] = MakeString("DLI2361");
        putRequest1.attributes["ean"] = MakeString("ABC");
        putRequest1.attributes["webSite"] = MakeString("http://DLI2361/public");
        putRequest1.attributes["town"] = MakeString("Hamburg");
        putRequest1.attributes["street"] = MakeString("Elbchaussee");
        putRequest1.attributes["streetNo"] = MakeNumber(265);
        _service.PutItem(putRequest1);

        Dto::DynamoDb::PutItemRequest putRequest2;
        putRequest2.region = REGION;
        putRequest2.tableName = TABLE_NAME;
        putRequest2.attributes["datenlieferantId"] = MakeString("DLI2361");
        putRequest2.attributes["ean"] = MakeString("DEF");
        putRequest2.attributes["webSite"] = MakeString("http://DLI2361/private");
        putRequest2.attributes["town"] = MakeString("Berlin");
        putRequest2.attributes["street"] = MakeString("Einsteinstrasse");
        putRequest2.attributes["streetNo"] = MakeNumber(32);
        _service.PutItem(putRequest2);

        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = REGION;
        queryRequest.tableName = TABLE_NAME;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId AND #AMZN_MAPPED_ean = :AMZN_MAPPED_ean";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_ean"] = "ean";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = MakeString("DLI2361");
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_ean"] = MakeString("ABC");

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _service.Query(queryRequest);

        // assert
        EXPECT_FALSE(queryResponse.tableName.empty());
        EXPECT_FALSE(queryResponse.items.empty());
        EXPECT_EQ(1, queryResponse.items.size());
        EXPECT_EQ("ABC", queryResponse.items.at(0).at("ean").stringValue);
    }

    TEST_F(DynamoDbServiceTest, QueryItemTest3) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::PutItemRequest putRequest1;
        putRequest1.region = REGION;
        putRequest1.tableName = TABLE_NAME;
        putRequest1.attributes["datenlieferantId"] = MakeString("DLI2361");
        putRequest1.attributes["ean"] = MakeString("ABC");
        putRequest1.attributes["webSite"] = MakeString("http://DLI2361/public");
        putRequest1.attributes["town"] = MakeString("Hamburg");
        putRequest1.attributes["street"] = MakeString("Elbchaussee");
        putRequest1.attributes["streetNo"] = MakeNumber(265);
        _service.PutItem(putRequest1);

        Dto::DynamoDb::PutItemRequest putRequest2;
        putRequest2.region = REGION;
        putRequest2.tableName = TABLE_NAME;
        putRequest2.attributes["datenlieferantId"] = MakeString("DLI2361");
        putRequest2.attributes["ean"] = MakeString("DEF");
        putRequest2.attributes["webSite"] = MakeString("http://DLI2361/private");
        putRequest2.attributes["town"] = MakeString("Berlin");
        putRequest2.attributes["street"] = MakeString("Einsteinstrasse");
        putRequest2.attributes["streetNo"] = MakeNumber(32);
        _service.PutItem(putRequest2);

        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = REGION;
        queryRequest.tableName = TABLE_NAME;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId AND #AMZN_MAPPED_ean > :AMZN_MAPPED_ean";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_ean"] = "ean";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = MakeString("DLI2361");
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_ean"] = MakeString("ABC");

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _service.Query(queryRequest);

        // assert
        EXPECT_FALSE(queryResponse.tableName.empty());
        EXPECT_FALSE(queryResponse.items.empty());
        EXPECT_EQ(1, queryResponse.items.size());
        EXPECT_EQ("DEF", queryResponse.items.at(0).at("ean").stringValue);
    }

    TEST_F(DynamoDbServiceTest, QueryItemTest4) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::PutItemRequest putRequest1;
        putRequest1.region = REGION;
        putRequest1.tableName = TABLE_NAME;
        putRequest1.attributes["datenlieferantId"] = MakeString("DLI2361");
        putRequest1.attributes["ean"] = MakeString("ABC");
        putRequest1.attributes["webSite"] = MakeString("http://DLI2361/public");
        putRequest1.attributes["town"] = MakeString("Hamburg");
        putRequest1.attributes["street"] = MakeString("Elbchaussee");
        putRequest1.attributes["streetNo"] = MakeNumber(265);
        _service.PutItem(putRequest1);

        Dto::DynamoDb::PutItemRequest putRequest2;
        putRequest2.region = REGION;
        putRequest2.tableName = TABLE_NAME;
        putRequest2.attributes["datenlieferantId"] = MakeString("DLI2361");
        putRequest2.attributes["ean"] = MakeString("DEF");
        putRequest2.attributes["webSite"] = MakeString("http://DLI2361/private");
        putRequest2.attributes["town"] = MakeString("Berlin");
        putRequest2.attributes["street"] = MakeString("Einsteinstrasse");
        putRequest2.attributes["streetNo"] = MakeNumber(32);
        _service.PutItem(putRequest2);

        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = REGION;
        queryRequest.tableName = TABLE_NAME;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId AND #AMZN_MAPPED_ean < :AMZN_MAPPED_ean";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_ean"] = "ean";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = MakeString("DLI2361");
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_ean"] = MakeString("DEF");

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _service.Query(queryRequest);

        // assert
        EXPECT_FALSE(queryResponse.tableName.empty());
        EXPECT_FALSE(queryResponse.items.empty());
        EXPECT_EQ(1, queryResponse.items.size());
        EXPECT_EQ("ABC", queryResponse.items.at(0).at("ean").stringValue);
    }

    TEST_F(DynamoDbServiceTest, QueryItemTest5) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::PutItemRequest putRequest1;
        putRequest1.region = REGION;
        putRequest1.tableName = TABLE_NAME;
        putRequest1.attributes["datenlieferantId"] = MakeString("DLI2361");
        putRequest1.attributes["ean"] = MakeString("ABC");
        putRequest1.attributes["webSite"] = MakeString("http://DLI2361/public");
        putRequest1.attributes["town"] = MakeString("Hamburg");
        putRequest1.attributes["street"] = MakeString("Elbchaussee");
        putRequest1.attributes["streetNo"] = MakeNumber(265);
        _service.PutItem(putRequest1);

        Dto::DynamoDb::PutItemRequest putRequest2;
        putRequest2.region = REGION;
        putRequest2.tableName = TABLE_NAME;
        putRequest2.attributes["datenlieferantId"] = MakeString("DLI2361");
        putRequest2.attributes["ean"] = MakeString("DEF");
        putRequest2.attributes["webSite"] = MakeString("http://DLI2361/private");
        putRequest2.attributes["town"] = MakeString("Berlin");
        putRequest2.attributes["street"] = MakeString("Einsteinstrasse");
        putRequest2.attributes["streetNo"] = MakeNumber(32);
        _service.PutItem(putRequest2);

        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = REGION;
        queryRequest.tableName = TABLE_NAME;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId AND #AMZN_MAPPED_ean >= :AMZN_MAPPED_ean";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_ean"] = "ean";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = MakeString("DLI2361");
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_ean"] = MakeString("ABC");

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _service.Query(queryRequest);

        // assert
        EXPECT_FALSE(queryResponse.tableName.empty());
        EXPECT_FALSE(queryResponse.items.empty());
        EXPECT_EQ(2, queryResponse.items.size());
        EXPECT_EQ("ABC", queryResponse.items.at(0).at("ean").stringValue);
        EXPECT_EQ("DEF", queryResponse.items.at(1).at("ean").stringValue);
    }

    TEST_F(DynamoDbServiceTest, QueryItemTest6) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::PutItemRequest putRequest1;
        putRequest1.region = REGION;
        putRequest1.tableName = TABLE_NAME;
        putRequest1.attributes["datenlieferantId"] = MakeString("DLI2361");
        putRequest1.attributes["ean"] = MakeString("ABC");
        putRequest1.attributes["webSite"] = MakeString("http://DLI2361/public");
        putRequest1.attributes["town"] = MakeString("Hamburg");
        putRequest1.attributes["street"] = MakeString("Elbchaussee");
        putRequest1.attributes["streetNo"] = MakeNumber(265);
        _service.PutItem(putRequest1);

        Dto::DynamoDb::PutItemRequest putRequest2;
        putRequest2.region = REGION;
        putRequest2.tableName = TABLE_NAME;
        putRequest2.attributes["datenlieferantId"] = MakeString("DLI2361");
        putRequest2.attributes["ean"] = MakeString("DEF");
        putRequest2.attributes["webSite"] = MakeString("http://DLI2361/private");
        putRequest2.attributes["town"] = MakeString("Berlin");
        putRequest2.attributes["street"] = MakeString("Einsteinstrasse");
        putRequest2.attributes["streetNo"] = MakeNumber(32);
        _service.PutItem(putRequest2);

        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = REGION;
        queryRequest.tableName = TABLE_NAME;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId AND #AMZN_MAPPED_ean <= :AMZN_MAPPED_ean";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_ean"] = "ean";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = MakeString("DLI2361");
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_ean"] = MakeString("DEF");

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _service.Query(queryRequest);

        // assert
        EXPECT_FALSE(queryResponse.tableName.empty());
        EXPECT_FALSE(queryResponse.items.empty());
        EXPECT_EQ(2, queryResponse.items.size());
        EXPECT_EQ("ABC", queryResponse.items.at(0).at("ean").stringValue);
        EXPECT_EQ("DEF", queryResponse.items.at(1).at("ean").stringValue);
    }

    TEST_F(DynamoDbServiceTest, DeleteItemTest) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        _service.CreateTable(createRequest);

        Dto::DynamoDb::PutItemRequest putRequest;
        putRequest.region = REGION;
        putRequest.tableName = TABLE_NAME;
        putRequest.attributes["datenlieferantId"] = MakeString("DLI333");
        putRequest.attributes["ean"] = MakeString("ABC");
        putRequest.attributes["webSite"] = MakeString("http://DLI333/public");
        putRequest.attributes["town"] = MakeString("Hamburg");
        putRequest.attributes["street"] = MakeString("Elbchaussee");
        putRequest.attributes["streetNo"] = MakeNumber(265);
        _service.PutItem(putRequest);

        Dto::DynamoDb::DeleteItemRequest deleteRequest;
        deleteRequest.region = REGION;
        deleteRequest.tableName = TABLE_NAME;
        deleteRequest.keys["datenlieferantId"] = MakeString("DLI333");
        deleteRequest.keys["ean"] = MakeString("ABC");

        // act
        const Dto::DynamoDb::DeleteItemResponse deleteResponse = _service.DeleteItem(deleteRequest);

        // assert
        Dto::DynamoDb::GetItemRequest getRequest;
        getRequest.region = REGION;
        getRequest.tableName = TABLE_NAME;
        getRequest.keys["datenlieferantId"] = MakeString("DLI333");
        getRequest.keys["ean"] = MakeString("ABC");
        const Dto::DynamoDb::GetItemResponse getResponse = _service.GetItem(getRequest);
        EXPECT_TRUE(getResponse.attributes.empty());
    }

    TEST_F(DynamoDbServiceTest, DeleteTableTest) {

        // arrange
        const Dto::DynamoDb::CreateTableRequest createRequest = CreateDefaultTableRequest();
        const Dto::DynamoDb::CreateTableResponse createResponse = _service.CreateTable(createRequest);
        EXPECT_FALSE(createResponse.tableArn.empty());
        EXPECT_EQ(TABLE_NAME, createResponse.tableName);

        Dto::DynamoDb::DeleteTableRequest deleteRequest;
        deleteRequest.region = REGION;
        deleteRequest.tableName = TABLE_NAME;

        // act
        const Dto::DynamoDb::DeleteTableResponse deleteResponse = _service.DeleteTable(deleteRequest);

        // assert
        EXPECT_FALSE(deleteResponse.tableArn.empty());
        EXPECT_EQ(TABLE_NAME, deleteResponse.tableName);
    }

}// namespace Awsmock::Service

#endif// AWMOCK_SERVICE_DYANMODB_SERVICE_TEST_H
