//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/entity/dynamodb/AttributeDefinition.h>
#include <awsmock/repository/DynamoDbDatabase.h>

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_TABLE_NAME "test-table"
#define TEST_TABLE_REGION "eu-cental-1"

namespace AwsMock::Database {

    Entity::DynamoDb::Item CreateDefaultItem(const std::string &region, const std::string &tableName) {
        Entity::DynamoDb::Item item;
        item.tableName = tableName;
        item.region = region;

        item.partitionKey.emplace<std::string>("test-value-1");
        item.attributes["test-attribute-1"].value.emplace<std::string>("test-value-1");
        item.attributes["test-attribute-2"].value.emplace<std::string>("test-value-2");
        return item;
    }

    Entity::DynamoDb::Table CreateDefaultTable() {
        Entity::DynamoDb::Table table;
        table.region = TEST_TABLE_REGION;
        table.name = TEST_TABLE_NAME;
        table.arn = Core::AwsUtils::CreateDynamoDbTableArn(TEST_ACCOUNT_ID, TEST_TABLE_NAME);

        // arrange attributes
        Entity::DynamoDb::AttributeDefinition attributeDefinition1;
        attributeDefinition1.attributeName = "test-attribute-1";
        attributeDefinition1.attributeType = "S";
        table.attributeDefinitions.push_back(attributeDefinition1);
        Entity::DynamoDb::AttributeDefinition attributeDefinition2;
        attributeDefinition2.attributeName = "test-attribute-2";
        attributeDefinition2.attributeType = "N";
        table.attributeDefinitions.push_back(attributeDefinition2);

        // arrange key schema
        Entity::DynamoDb::KeySchema keySchema1;
        keySchema1.attributeName = "test-attribute-1";
        keySchema1.keyType = "HASH";
        table.keySchema.emplace_back(keySchema1);
        return table;
    }

    struct S3DynamoDbFixture {
        S3DynamoDbFixture() = default;

        ~S3DynamoDbFixture() {
            const long objectCount = DynamoDbDatabase::instance().DeleteAllItems();
            log_debug << "Items deleted " << objectCount;
            const long bucketCount = DynamoDbDatabase::instance().DeleteAllTables();
            log_debug << "Tables deleted " << bucketCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(S3DynamoDbTests, S3DynamoDbFixture)

    BOOST_AUTO_TEST_CASE(CreateTableTest) {

        // arrange
        const DynamoDbDatabase &dynamoDbDatabase = DynamoDbDatabase::instance();
        Entity::DynamoDb::Table table = CreateDefaultTable();

        // act
        table = dynamoDbDatabase.CreateTable(table);

        // assert
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());
        BOOST_CHECK_EQUAL(false, table.keySchema.empty());
        BOOST_CHECK_EQUAL(false, table.attributeDefinitions.empty());
    }

    BOOST_AUTO_TEST_CASE(ListTableTest) {

        // arrange
        const DynamoDbDatabase &dynamoDbDatabase = DynamoDbDatabase::instance();
        Entity::DynamoDb::Table table = CreateDefaultTable();
        table = dynamoDbDatabase.CreateTable(table);
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());

        // act
        const std::vector<Entity::DynamoDb::Table> tables = dynamoDbDatabase.ListTables();

        // assert
        BOOST_CHECK_EQUAL(false, tables.empty());
    }

    BOOST_AUTO_TEST_CASE(DeleteAllTablesTest) {

        // arrange
        const DynamoDbDatabase &dynamoDbDatabase = DynamoDbDatabase::instance();
        Entity::DynamoDb::Table table = CreateDefaultTable();
        table = dynamoDbDatabase.CreateTable(table);
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());

        // act
        const long result = dynamoDbDatabase.DeleteAllTables();

        // assert
        BOOST_CHECK_EQUAL(true, result > 0);
    }

    BOOST_AUTO_TEST_CASE(CreateItemTest) {

        // arrange
        const DynamoDbDatabase &dynamoDbDatabase = DynamoDbDatabase::instance();
        Entity::DynamoDb::Table table = CreateDefaultTable();
        table = dynamoDbDatabase.CreateTable(table);
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());
        Entity::DynamoDb::Item item = CreateDefaultItem(table.region, table.name);

        // act
        item = dynamoDbDatabase.CreateItem(item);

        // assert
        BOOST_CHECK_EQUAL(false, item.attributes.empty());
        BOOST_CHECK_EQUAL(table.name, item.tableName);
    }

    BOOST_AUTO_TEST_CASE(GetItemTest) {

        // arrange
        const DynamoDbDatabase &dynamoDbDatabase = DynamoDbDatabase::instance();
        Entity::DynamoDb::Table table = CreateDefaultTable();
        table = dynamoDbDatabase.CreateTable(table);
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());
        Entity::DynamoDb::Item item = CreateDefaultItem(table.region, table.name);
        item = dynamoDbDatabase.CreateItem(item);
        BOOST_CHECK_EQUAL(false, item.oid.empty());

        // act
        item = dynamoDbDatabase.GetItemByKeys(table.region, table.name, "test-value-1");

        // assert
        BOOST_CHECK_EQUAL(false, item.attributes.empty());
        BOOST_CHECK_EQUAL(std::get<0>(item.attributes["test-attribute-1"].value), "test-value-1");
        BOOST_CHECK_EQUAL(std::get<0>(item.attributes["test-attribute-2"].value), "test-value-2");
    }

    BOOST_AUTO_TEST_CASE(ListItemTest) {

        // arrange
        const DynamoDbDatabase &dynamoDbDatabase = DynamoDbDatabase::instance();
        Entity::DynamoDb::Table table = CreateDefaultTable();
        table = dynamoDbDatabase.CreateTable(table);
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());
        Entity::DynamoDb::Item item = CreateDefaultItem(table.region, table.name);
        item = dynamoDbDatabase.CreateItem(item);
        BOOST_CHECK_EQUAL(false, item.oid.empty());

        // act
        const std::vector<Entity::DynamoDb::Item> items = dynamoDbDatabase.ListItems(table.region, table.name);

        // assert
        BOOST_CHECK_EQUAL(false, items.empty());
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace AwsMock::Database