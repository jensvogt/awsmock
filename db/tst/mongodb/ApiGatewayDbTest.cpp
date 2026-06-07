// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/repository/RepositoryFactory.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_REGION "eu-central-1"
#define TEST_KEY_NAME "test-api-key"
#define TEST_KEY_ID "abc123"
#define TEST_KEY_VALUE "test-key-value-0123456789"
#define TEST_CUSTOMER_ID "customer-001"
#define TEST_REST_API_NAME "test-rest-api"
#define TEST_REST_API_ID "rest-001"

namespace Awsmock::Database {

    Entity::ApiGateway::ApiKey CreateDefaultApiKey(const std::string &region, const std::string &name, const std::string &id) {
        Entity::ApiGateway::ApiKey key;
        key.region = region;
        key.name = name;
        key.id = id;
        key.customerId = TEST_CUSTOMER_ID;
        key.description = "test description";
        key.enabled = true;
        key.generateDistinct = false;
        key.keyValue = TEST_KEY_VALUE;
        return key;
    }

    Entity::ApiGateway::RestApi CreateDefaultRestApi(const std::string &region, const std::string &name, const std::string &id) {
        Entity::ApiGateway::RestApi restApi;
        restApi.region = region;
        restApi.name = name;
        restApi.id = id;
        restApi.description = "test rest api";
        restApi.apiKeySource = Entity::ApiGateway::HEADER;
        restApi.version = "1.0";
        restApi.disableExecuteApiEndpoint = false;
        restApi.minimumCompressionSize = 0;
        return restApi;
    }

    struct ApiGatewayMongoDbFixture {
        ApiGatewayMongoDbFixture() {
            RepositoryFactory::instance().initialize(BackendType::MONGODB);
        }
        ~ApiGatewayMongoDbFixture() {
            const long count = RepositoryFactory::instance().apigatewayRepository()->deleteAllKeys();
            log_debug << "API keys deleted, count: " << count;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(ApiGatewayMongoDbTests, ApiGatewayMongoDbFixture)

    // =============================================================================================================
    // API key tests
    // =============================================================================================================

    BOOST_AUTO_TEST_CASE(ApiKeyCreate) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::ApiKey key = CreateDefaultApiKey(TEST_REGION, TEST_KEY_NAME, TEST_KEY_ID);

        // act
        const Entity::ApiGateway::ApiKey result = repo->createKey(key);

        // assert
        BOOST_CHECK_EQUAL(result.name, TEST_KEY_NAME);
        BOOST_CHECK_EQUAL(result.id, TEST_KEY_ID);
        BOOST_CHECK_EQUAL(result.region, TEST_REGION);
        BOOST_CHECK_EQUAL(result.customerId, TEST_CUSTOMER_ID);
        BOOST_CHECK_EQUAL(result.enabled, true);
        BOOST_CHECK(!result.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(ApiKeyExistsById) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::ApiKey key = CreateDefaultApiKey(TEST_REGION, TEST_KEY_NAME, TEST_KEY_ID);
        key = repo->createKey(key);

        // act
        const bool result = repo->apiKeyExists(TEST_KEY_ID);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(ApiKeyExistsByRegionName) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::ApiKey key = CreateDefaultApiKey(TEST_REGION, TEST_KEY_NAME, TEST_KEY_ID);
        key = repo->createKey(key);

        // act
        const bool result = repo->apiKeyExists(TEST_REGION, TEST_KEY_NAME);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(ApiKeyGetById) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::ApiKey key = CreateDefaultApiKey(TEST_REGION, TEST_KEY_NAME, TEST_KEY_ID);
        key = repo->createKey(key);

        // act
        const Entity::ApiGateway::ApiKey result = repo->getApiKeyById(TEST_KEY_ID);

        // assert
        BOOST_CHECK_EQUAL(result.name, TEST_KEY_NAME);
        BOOST_CHECK_EQUAL(result.id, TEST_KEY_ID);
        BOOST_CHECK_EQUAL(result.region, TEST_REGION);
    }

    BOOST_AUTO_TEST_CASE(ApiKeyGetAll) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::ApiKey key1 = CreateDefaultApiKey(TEST_REGION, "key-one", "id-001");
        Entity::ApiGateway::ApiKey key2 = CreateDefaultApiKey(TEST_REGION, "key-two", "id-002");
        key1 = repo->createKey(key1);
        key2 = repo->createKey(key2);

        // act
        const std::vector<Entity::ApiGateway::ApiKey> result = repo->listApiKeys("", "", "", 100);

        // assert
        BOOST_CHECK_EQUAL(2, result.size());
    }

    BOOST_AUTO_TEST_CASE(ApiKeyGetByNameQuery) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::ApiKey key1 = CreateDefaultApiKey(TEST_REGION, "key-one", "id-001");
        Entity::ApiGateway::ApiKey key2 = CreateDefaultApiKey(TEST_REGION, "other-key", "id-002");
        key1 = repo->createKey(key1);
        key2 = repo->createKey(key2);

        // act
        const std::vector<Entity::ApiGateway::ApiKey> result = repo->listApiKeys("key-one", "", "", 100);

        // assert
        BOOST_CHECK_EQUAL(1, result.size());
        BOOST_CHECK_EQUAL(result[0].name, "key-one");
    }

    BOOST_AUTO_TEST_CASE(ApiKeyUpdate) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::ApiKey key = CreateDefaultApiKey(TEST_REGION, TEST_KEY_NAME, TEST_KEY_ID);
        key = repo->createKey(key);

        // act
        key.description = "updated description";
        const Entity::ApiGateway::ApiKey result = repo->updateApiKey(key);

        // assert
        BOOST_CHECK_EQUAL(result.description, "updated description");
        BOOST_CHECK_EQUAL(result.name, TEST_KEY_NAME);
    }

    BOOST_AUTO_TEST_CASE(ApiKeyCount) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::ApiKey key1 = CreateDefaultApiKey(TEST_REGION, "key-one", "id-001");
        Entity::ApiGateway::ApiKey key2 = CreateDefaultApiKey(TEST_REGION, "key-two", "id-002");
        key1 = repo->createKey(key1);
        key2 = repo->createKey(key2);

        // act
        const long result = repo->countApiKeys();

        // assert
        BOOST_CHECK_EQUAL(2, result);
    }

    BOOST_AUTO_TEST_CASE(ApiKeyDelete) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::ApiKey key = CreateDefaultApiKey(TEST_REGION, TEST_KEY_NAME, TEST_KEY_ID);
        key = repo->createKey(key);
        BOOST_CHECK_EQUAL(repo->apiKeyExists(TEST_KEY_ID), true);

        // act
        repo->deleteKey(TEST_KEY_ID);

        // assert
        BOOST_CHECK_EQUAL(repo->apiKeyExists(TEST_KEY_ID), false);
    }

    BOOST_AUTO_TEST_CASE(ApiKeyDeleteAll) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::ApiKey key1 = CreateDefaultApiKey(TEST_REGION, "key-one", "id-001");
        Entity::ApiGateway::ApiKey key2 = CreateDefaultApiKey(TEST_REGION, "key-two", "id-002");
        key1 = repo->createKey(key1);
        key2 = repo->createKey(key2);
        BOOST_CHECK_EQUAL(repo->countApiKeys(), 2);

        // act
        const long deleted = repo->deleteAllKeys();

        // assert
        BOOST_CHECK_EQUAL(deleted, 2);
        BOOST_CHECK_EQUAL(repo->countApiKeys(), 0);
    }

    BOOST_AUTO_TEST_CASE(ApiKeyListCounters) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::ApiKey key1 = CreateDefaultApiKey(TEST_REGION, "key-alpha", "id-001");
        Entity::ApiGateway::ApiKey key2 = CreateDefaultApiKey(TEST_REGION, "key-beta", "id-002");
        key1 = repo->createKey(key1);
        key2 = repo->createKey(key2);

        // act
        const std::vector<Entity::ApiGateway::ApiKey> result = repo->listApiKeyCounters("", 2, 0, {});

        // assert
        BOOST_CHECK_EQUAL(2, result.size());
    }

    // =============================================================================================================
    // REST API tests
    // =============================================================================================================

    BOOST_AUTO_TEST_CASE(RestApiCreate) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::RestApi restApi = CreateDefaultRestApi(TEST_REGION, TEST_REST_API_NAME, TEST_REST_API_ID);

        // act
        const Entity::ApiGateway::RestApi result = repo->createRestApi(restApi);

        // assert
        BOOST_CHECK_EQUAL(result.name, TEST_REST_API_NAME);
        BOOST_CHECK_EQUAL(result.region, TEST_REGION);
        BOOST_CHECK_EQUAL(result.description, "test rest api");
        BOOST_CHECK(!result.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(RestApiExistsByRegionName) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::RestApi restApi = CreateDefaultRestApi(TEST_REGION, TEST_REST_API_NAME, TEST_REST_API_ID);
        restApi = repo->createRestApi(restApi);

        // act
        const bool result = repo->restApiExists(TEST_REGION, TEST_REST_API_NAME);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(RestApiExistsById) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::RestApi restApi = CreateDefaultRestApi(TEST_REGION, TEST_REST_API_NAME, TEST_REST_API_ID);
        const Entity::ApiGateway::RestApi created = repo->createRestApi(restApi);

        // act
        const bool result = repo->restApiExists(created.oid);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(RestApiListCounters) {

        // arrange
        const std::shared_ptr<IApiGatewayRepository> repo = RepositoryFactory::instance().apigatewayRepository();
        Entity::ApiGateway::RestApi api1 = CreateDefaultRestApi(TEST_REGION, "api-alpha", "r-001");
        Entity::ApiGateway::RestApi api2 = CreateDefaultRestApi(TEST_REGION, "api-beta", "r-002");
        api1 = repo->createRestApi(api1);
        api2 = repo->createRestApi(api2);

        // act
        const std::vector<Entity::ApiGateway::RestApi> result = repo->listRestApiCounters("", 2, 0, {});

        // assert
        BOOST_CHECK_EQUAL(2, result.size());
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
