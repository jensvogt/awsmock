//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/entity/dynamodb/AttributeDefinition.h>
#include <awsmock/repository/DynamoDbDatabase.h>
#include <awsmock/service/dynamodb/DynamoDbService.h>

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_TABLE "test-table"
#define TEST_REGION "eu-cental-1"

namespace AwsMock::Database {

    Dto::DynamoDb::CreateTableRequest CreateDefaultTableRequest() {

        Dto::DynamoDb::CreateTableRequest request;
        request.region = TEST_REGION;
        request.tableName = TEST_TABLE;

        std::vector<Dto::DynamoDb::AttributeDefinition> attributes;

        // First attribute
        Dto::DynamoDb::AttributeDefinition attribute1;
        attribute1.attributeName = "test-attribute-1";
        attribute1.attributeType = "S";
        attributes.push_back(attribute1);

        // First attribute
        Dto::DynamoDb::AttributeDefinition attribute2;
        attribute2.attributeName = "test-attribute-2";
        attribute2.attributeType = "S";
        attributes.push_back(attribute2);

        // First attribute
        Dto::DynamoDb::AttributeDefinition attribute3;
        attribute3.attributeName = "test-attribute-3";
        attribute3.attributeType = "S";
        attributes.push_back(attribute3);

        // Key schemas
        std::vector<Dto::DynamoDb::KeySchema> keySchemas;

        Dto::DynamoDb::KeySchema keySchema1;
        keySchema1.attributeName = "test-attribute-1";
        keySchema1.keyType = "HASH";
        keySchemas.push_back(keySchema1);

        Dto::DynamoDb::KeySchema keySchema2;
        keySchema2.attributeName = "test-attribute-2";
        keySchema2.keyType = "RANGE";
        keySchemas.push_back(keySchema2);

        request.attributes = attributes;
        request.keySchema = keySchemas;

        return request;
    }

    Dto::DynamoDb::AttributeValue CreateDefaultAttributeValue(const std::string &value) {
        Dto::DynamoDb::AttributeValue attribute;
        attribute.type = "S";
        attribute.stringValue = value;
        return attribute;
    }

    Dto::DynamoDb::AttributeValue CreateDefaultAttributeValue(const int &value) {
        Dto::DynamoDb::AttributeValue attribute;
        attribute.type = "N";
        attribute.numberValue = std::to_string(value);
        return attribute;
    }

    std::vector<Dto::DynamoDb::AttributeValue> CreateDefaultAttributes(const std::string &attributeName, const std::string &value) {
        std::vector<Dto::DynamoDb::AttributeValue> attributes;
        attributes.push_back(CreateDefaultAttributeValue(value));
        return attributes;
    }

    struct DynamoDbServiceFixture {
        DynamoDbServiceFixture() = default;
        ~DynamoDbServiceFixture() {
            const Service::DynamoDbService _dynamoDbService;
            const long deletedTables = _dynamoDbService.DeleteAllTables();
            log_debug << "Tables deleted, count: " << deletedTables;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(DynamoDbServiceTests, DynamoDbServiceFixture)

    BOOST_AUTO_TEST_CASE(CreateTableTest) {

        // arrange
        const Service::DynamoDbService _dynamoDbService;
        const Dto::DynamoDb::CreateTableRequest request = CreateDefaultTableRequest();

        // act
        const Dto::DynamoDb::CreateTableResponse response = _dynamoDbService.CreateTable(request);

        // assert
        BOOST_CHECK_EQUAL(false, response.tableArn.empty());
        BOOST_CHECK_EQUAL(false, response.tableName.empty());
        BOOST_CHECK_EQUAL(false, response.keySchemas.empty());
        BOOST_CHECK_EQUAL(false, response.attributeDefinitions.empty());
    }

    BOOST_AUTO_TEST_CASE(PutItemTest) {

        // arrange
        const Service::DynamoDbService _dynamoDbService;
        const Dto::DynamoDb::CreateTableRequest request = CreateDefaultTableRequest();
        const Dto::DynamoDb::CreateTableResponse response = _dynamoDbService.CreateTable(request);
        Dto::DynamoDb::PutItemRequest putRequest;
        putRequest.region = TEST_REGION;
        putRequest.tableName = TEST_TABLE;
        putRequest.keys["test-key-1"] = CreateDefaultAttributeValue("test-value-1");
        putRequest.keys["test-key-2"] = CreateDefaultAttributeValue("test-value-2");
        putRequest.attributes["test-attribute-1"] = CreateDefaultAttributeValue("test-value-1");
        putRequest.attributes["test-attribute-2"] = CreateDefaultAttributeValue("test-value-2");
        putRequest.attributes["test-attribute-3"] = CreateDefaultAttributeValue("test-value-3");

        // act
        const Dto::DynamoDb::PutItemResponse putResponse = _dynamoDbService.PutItem(putRequest);

        // assert
        BOOST_CHECK_EQUAL(false, putResponse.item.tableName.empty());
        BOOST_CHECK_EQUAL(false, putResponse.item.attributes.empty());
        BOOST_CHECK_EQUAL(false, putResponse.item.keys.empty());
    }

    BOOST_AUTO_TEST_CASE(QueryItemTest1) {

        // arrange
        const Service::DynamoDbService _dynamoDbService;
        const Dto::DynamoDb::CreateTableRequest request = CreateDefaultTableRequest();
        const Dto::DynamoDb::CreateTableResponse response = _dynamoDbService.CreateTable(request);
        Dto::DynamoDb::PutItemRequest putRequest;
        putRequest.region = TEST_REGION;
        putRequest.tableName = TEST_TABLE;
        putRequest.keys["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest.attributes["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        const Dto::DynamoDb::PutItemResponse putResponse = _dynamoDbService.PutItem(putRequest);

        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = TEST_REGION;
        queryRequest.tableName = TEST_TABLE;
        queryRequest.limit = 10;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _dynamoDbService.Query(queryRequest);

        // assert
        BOOST_CHECK_EQUAL(false, queryResponse.tableName.empty());
        BOOST_CHECK_EQUAL(false, queryResponse.items.empty());
    }

    BOOST_AUTO_TEST_CASE(QueryItemTest2) {

        // arrange
        const Service::DynamoDbService _dynamoDbService;
        const Dto::DynamoDb::CreateTableRequest request = CreateDefaultTableRequest();
        const Dto::DynamoDb::CreateTableResponse response = _dynamoDbService.CreateTable(request);

        // Value1
        Dto::DynamoDb::PutItemRequest putRequest1;
        putRequest1.region = TEST_REGION;
        putRequest1.tableName = TEST_TABLE;
        putRequest1.keys["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest1.attributes["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest1.attributes["value"] = CreateDefaultAttributeValue(1);
        Dto::DynamoDb::PutItemResponse putResponse = _dynamoDbService.PutItem(putRequest1);
        BOOST_CHECK_EQUAL(TEST_TABLE, putResponse.item.tableName);

        // Value1
        Dto::DynamoDb::PutItemRequest putRequest2;
        putRequest2.region = TEST_REGION;
        putRequest2.tableName = TEST_TABLE;
        putRequest2.keys["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest2.attributes["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest2.attributes["value"] = CreateDefaultAttributeValue(2);
        putResponse = _dynamoDbService.PutItem(putRequest2);
        BOOST_CHECK_EQUAL(TEST_TABLE, putResponse.item.tableName);

        // Query
        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = TEST_REGION;
        queryRequest.tableName = TEST_TABLE;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId AND #AMZN_MAPPED_value = :AMZN_MAPPED_value";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_value"] = "value";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_value"] = CreateDefaultAttributeValue(2);

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _dynamoDbService.Query(queryRequest);

        // assert
        BOOST_CHECK_EQUAL(false, queryResponse.tableName.empty());
        BOOST_CHECK_EQUAL(false, queryResponse.items.empty());
    }

    BOOST_AUTO_TEST_CASE(QueryItemTest3) {

        // arrange
        const Service::DynamoDbService _dynamoDbService;
        const Dto::DynamoDb::CreateTableRequest request = CreateDefaultTableRequest();
        const Dto::DynamoDb::CreateTableResponse response = _dynamoDbService.CreateTable(request);

        // Value1
        Dto::DynamoDb::PutItemRequest putRequest1;
        putRequest1.region = TEST_REGION;
        putRequest1.tableName = TEST_TABLE;
        putRequest1.keys["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest1.attributes["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest1.attributes["value"] = CreateDefaultAttributeValue(1);
        Dto::DynamoDb::PutItemResponse putResponse = _dynamoDbService.PutItem(putRequest1);
        BOOST_CHECK_EQUAL(TEST_TABLE, putResponse.item.tableName);

        // Value1
        Dto::DynamoDb::PutItemRequest putRequest2;
        putRequest2.region = TEST_REGION;
        putRequest2.tableName = TEST_TABLE;
        putRequest2.keys["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest2.attributes["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest2.attributes["value"] = CreateDefaultAttributeValue(2);
        putResponse = _dynamoDbService.PutItem(putRequest2);
        BOOST_CHECK_EQUAL(TEST_TABLE, putResponse.item.tableName);

        // Query
        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = TEST_REGION;
        queryRequest.tableName = TEST_TABLE;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId AND #AMZN_MAPPED_value > :AMZN_MAPPED_value";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_value"] = "value";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_value"] = CreateDefaultAttributeValue(1);

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _dynamoDbService.Query(queryRequest);

        // assert
        BOOST_CHECK_EQUAL(false, queryResponse.tableName.empty());
        BOOST_CHECK_EQUAL(false, queryResponse.items.empty());
        BOOST_CHECK_EQUAL("2", queryResponse.items.at(0).at("value").numberValue);
    }

    BOOST_AUTO_TEST_CASE(QueryItemTest4) {

        // arrange
        const Service::DynamoDbService _dynamoDbService;
        const Dto::DynamoDb::CreateTableRequest request = CreateDefaultTableRequest();
        const Dto::DynamoDb::CreateTableResponse response = _dynamoDbService.CreateTable(request);

        // Value1
        Dto::DynamoDb::PutItemRequest putRequest1;
        putRequest1.region = TEST_REGION;
        putRequest1.tableName = TEST_TABLE;
        putRequest1.keys["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest1.attributes["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest1.attributes["value"] = CreateDefaultAttributeValue(1);
        Dto::DynamoDb::PutItemResponse putResponse = _dynamoDbService.PutItem(putRequest1);
        BOOST_CHECK_EQUAL(TEST_TABLE, putResponse.item.tableName);

        // Value1
        Dto::DynamoDb::PutItemRequest putRequest2;
        putRequest2.region = TEST_REGION;
        putRequest2.tableName = TEST_TABLE;
        putRequest2.keys["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest2.attributes["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest2.attributes["value"] = CreateDefaultAttributeValue(1);
        putResponse = _dynamoDbService.PutItem(putRequest2);
        BOOST_CHECK_EQUAL(TEST_TABLE, putResponse.item.tableName);

        // Query
        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = TEST_REGION;
        queryRequest.tableName = TEST_TABLE;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId AND #AMZN_MAPPED_value < :AMZN_MAPPED_value";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_value"] = "value";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_value"] = CreateDefaultAttributeValue(2);

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _dynamoDbService.Query(queryRequest);

        // assert
        BOOST_CHECK_EQUAL(false, queryResponse.tableName.empty());
        BOOST_CHECK_EQUAL(false, queryResponse.items.empty());
        BOOST_CHECK_EQUAL("1", queryResponse.items.at(0).at("value").numberValue);
    }

    BOOST_AUTO_TEST_CASE(QueryItemTest5) {

        // arrange
        const Service::DynamoDbService _dynamoDbService;
        const Dto::DynamoDb::CreateTableRequest request = CreateDefaultTableRequest();
        const Dto::DynamoDb::CreateTableResponse response = _dynamoDbService.CreateTable(request);

        // Value1
        Dto::DynamoDb::PutItemRequest putRequest1;
        putRequest1.region = TEST_REGION;
        putRequest1.tableName = TEST_TABLE;
        putRequest1.keys["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest1.attributes["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest1.attributes["value"] = CreateDefaultAttributeValue(1);
        Dto::DynamoDb::PutItemResponse putResponse = _dynamoDbService.PutItem(putRequest1);
        BOOST_CHECK_EQUAL(TEST_TABLE, putResponse.item.tableName);

        // Value1
        Dto::DynamoDb::PutItemRequest putRequest2;
        putRequest2.region = TEST_REGION;
        putRequest2.tableName = TEST_TABLE;
        putRequest2.keys["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest2.attributes["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest2.attributes["value"] = CreateDefaultAttributeValue(2);
        putResponse = _dynamoDbService.PutItem(putRequest2);
        BOOST_CHECK_EQUAL(TEST_TABLE, putResponse.item.tableName);

        // Query
        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = TEST_REGION;
        queryRequest.tableName = TEST_TABLE;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId AND #AMZN_MAPPED_value >= :AMZN_MAPPED_value";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_value"] = "value";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_value"] = CreateDefaultAttributeValue(2);

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _dynamoDbService.Query(queryRequest);

        // assert
        BOOST_CHECK_EQUAL(false, queryResponse.tableName.empty());
        BOOST_CHECK_EQUAL(false, queryResponse.items.empty());
        BOOST_CHECK_EQUAL("2", queryResponse.items.at(0).at("value").numberValue);
    }

    BOOST_AUTO_TEST_CASE(QueryItemTest6) {

        // arrange
        const Service::DynamoDbService _dynamoDbService;
        const Dto::DynamoDb::CreateTableRequest request = CreateDefaultTableRequest();
        const Dto::DynamoDb::CreateTableResponse response = _dynamoDbService.CreateTable(request);

        // Value1
        Dto::DynamoDb::PutItemRequest putRequest1;
        putRequest1.region = TEST_REGION;
        putRequest1.tableName = TEST_TABLE;
        putRequest1.keys["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest1.keys["ean"] = CreateDefaultAttributeValue("ABC");
        putRequest1.attributes["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest1.attributes["value"] = CreateDefaultAttributeValue(1);
        Dto::DynamoDb::PutItemResponse putResponse = _dynamoDbService.PutItem(putRequest1);
        BOOST_CHECK_EQUAL(TEST_TABLE, putResponse.item.tableName);

        // Value1
        Dto::DynamoDb::PutItemRequest putRequest2;
        putRequest2.region = TEST_REGION;
        putRequest2.tableName = TEST_TABLE;
        putRequest2.keys["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest1.keys["ean"] = CreateDefaultAttributeValue("DEF");
        putRequest2.attributes["datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        putRequest2.attributes["value"] = CreateDefaultAttributeValue(1);
        putResponse = _dynamoDbService.PutItem(putRequest2);
        BOOST_CHECK_EQUAL(TEST_TABLE, putResponse.item.tableName);

        // Query
        Dto::DynamoDb::QueryRequest queryRequest;
        queryRequest.region = TEST_REGION;
        queryRequest.tableName = TEST_TABLE;
        queryRequest.keyConditionExpression = "#AMZN_MAPPED_datenlieferantId = :AMZN_MAPPED_datenlieferantId AND #AMZN_MAPPED_value <= :AMZN_MAPPED_value";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_datenlieferantId"] = "datenlieferantId";
        queryRequest.expressionAttributeNames["#AMZN_MAPPED_value"] = "value";
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_datenlieferantId"] = CreateDefaultAttributeValue("DLI2361");
        queryRequest.expressionAttributeValues[":AMZN_MAPPED_value"] = CreateDefaultAttributeValue(2);

        // act
        const Dto::DynamoDb::QueryResponse queryResponse = _dynamoDbService.Query(queryRequest);

        // assert
        BOOST_CHECK_EQUAL(false, queryResponse.tableName.empty());
        BOOST_CHECK_EQUAL(false, queryResponse.items.empty());
        BOOST_CHECK_EQUAL("1", queryResponse.items.at(0).at("value").numberValue);
    }

    BOOST_AUTO_TEST_CASE(DeleteTableTest) {

        // arrange
        const Service::DynamoDbService _dynamoDbService;
        const Dto::DynamoDb::CreateTableRequest request = CreateDefaultTableRequest();
        const Dto::DynamoDb::CreateTableResponse createResponse = _dynamoDbService.CreateTable(request);
        BOOST_CHECK_EQUAL(false, createResponse.tableArn.empty());
        BOOST_CHECK_EQUAL(false, createResponse.tableName.empty());
        Dto::DynamoDb::DeleteTableRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.tableName = TEST_TABLE;

        // act
        const Dto::DynamoDb::DeleteTableResponse response = _dynamoDbService.DeleteTable(deleteRequest);

        // assert
        BOOST_CHECK_EQUAL(false, response.tableArn.empty());
        BOOST_CHECK_EQUAL(false, response.tableName.empty());
    }
    BOOST_AUTO_TEST_SUITE_END()

}// namespace AwsMock::Database