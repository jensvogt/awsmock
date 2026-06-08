//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/entity/dynamodb/AttributeDefinition.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/repository/dynamodb/IDynamoDbRepository.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_TABLE_NAME "test-table"
#define TEST_TABLE_REGION "eu-cental-1"

namespace Awsmock::Database {

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
        S3DynamoDbFixture() {
            RepositoryFactory::instance().initialize(BackendType::MONGODB, "test");
        }
        ~S3DynamoDbFixture() {
            const long objectCount = RepositoryFactory::instance().dynamodbRepository()->deleteAllItems();
            log_debug << "Items deleted " << objectCount;
            const long bucketCount = RepositoryFactory::instance().dynamodbRepository()->deleteAllTables();
            log_debug << "Tables deleted " << bucketCount;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(S3DynamoDbTests, S3DynamoDbFixture)

    BOOST_AUTO_TEST_CASE(createTableTest) {

        // arrange
        const std::shared_ptr<IDynamoDbRepository> dynamoDbDatabase = RepositoryFactory::instance().dynamodbRepository();
        Entity::DynamoDb::Table table = CreateDefaultTable();

        // act
        table = dynamoDbDatabase->createTable(table);

        // assert
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());
        BOOST_CHECK_EQUAL(false, table.keySchema.empty());
        BOOST_CHECK_EQUAL(false, table.attributeDefinitions.empty());
    }

    BOOST_AUTO_TEST_CASE(ListTableTest) {

        // arrange
        const std::shared_ptr<IDynamoDbRepository> dynamoDbDatabase = RepositoryFactory::instance().dynamodbRepository();
        Entity::DynamoDb::Table table = CreateDefaultTable();
        table = dynamoDbDatabase->createTable(table);
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());

        // act
        const std::vector<Entity::DynamoDb::Table> tables = dynamoDbDatabase->listTables({}, {}, 0, 0, {});

        // assert
        BOOST_CHECK_EQUAL(false, tables.empty());
    }

    BOOST_AUTO_TEST_CASE(DeleteAllTablesTest) {

        // arrange
        const std::shared_ptr<IDynamoDbRepository> dynamoDbDatabase = RepositoryFactory::instance().dynamodbRepository();
        Entity::DynamoDb::Table table = CreateDefaultTable();
        table = dynamoDbDatabase->createTable(table);
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());

        // act
        const long result = dynamoDbDatabase->deleteAllTables();

        // assert
        BOOST_CHECK_EQUAL(true, result > 0);
    }

    BOOST_AUTO_TEST_CASE(createItemTest) {

        // arrange
        const std::shared_ptr<IDynamoDbRepository> dynamoDbDatabase = RepositoryFactory::instance().dynamodbRepository();
        Entity::DynamoDb::Table table = CreateDefaultTable();
        table = dynamoDbDatabase->createTable(table);
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());
        Entity::DynamoDb::Item item = CreateDefaultItem(table.region, table.name);

        // act
        item = dynamoDbDatabase->createItem(item);

        // assert
        BOOST_CHECK_EQUAL(false, item.attributes.empty());
        BOOST_CHECK_EQUAL(table.name, item.tableName);
    }

    BOOST_AUTO_TEST_CASE(GetItemTest) {

        // arrange
        const std::shared_ptr<IDynamoDbRepository> dynamoDbDatabase = RepositoryFactory::instance().dynamodbRepository();
        Entity::DynamoDb::Table table = CreateDefaultTable();
        table = dynamoDbDatabase->createTable(table);
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());
        Entity::DynamoDb::Item item = CreateDefaultItem(table.region, table.name);
        item = dynamoDbDatabase->createItem(item);
        BOOST_CHECK_EQUAL(false, item.oid.empty());

        // act
        item = dynamoDbDatabase->getItemByKeys(table.region, table.name, "test-value-1", {});

        // assert
        BOOST_CHECK_EQUAL(false, item.attributes.empty());
        BOOST_CHECK_EQUAL(std::get<0>(item.attributes["test-attribute-1"].value), "test-value-1");
        BOOST_CHECK_EQUAL(std::get<0>(item.attributes["test-attribute-2"].value), "test-value-2");
    }

    BOOST_AUTO_TEST_CASE(ListItemTest) {

        // arrange
        const std::shared_ptr<IDynamoDbRepository> dynamoDbDatabase = RepositoryFactory::instance().dynamodbRepository();
        Entity::DynamoDb::Table table = CreateDefaultTable();
        table = dynamoDbDatabase->createTable(table);
        BOOST_CHECK_EQUAL(false, table.arn.empty());
        BOOST_CHECK_EQUAL(false, table.oid.empty());
        Entity::DynamoDb::Item item = CreateDefaultItem(table.region, table.name);
        item = dynamoDbDatabase->createItem(item);
        BOOST_CHECK_EQUAL(false, item.oid.empty());

        // act
        const std::vector<Entity::DynamoDb::Item> items = dynamoDbDatabase->listItems(table.region, table.name, 0, 0, {});

        // assert
        BOOST_CHECK_EQUAL(false, items.empty());
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database