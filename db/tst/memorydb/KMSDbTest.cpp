//
// Created by vogje01 on 15/08/2026.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/repository/RepositoryFactory.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_REGION "eu-central-1"
#define TEST_KEY_ID "test-key-id"
#define TEST_KEY_USAGE "ENCRYPT_DECRYPT"
#define TEST_KEY_SPEC "SYMMETRIC_DEFAULT"
#define TEST_KEY_STATE "Enabled"
#define TEST_ORIGIN "AWS_KMS"

namespace Awsmock::Database {

    Entity::KMS::Key CreateDefaultKey(const std::string &region, const std::string &keyId) {
        Entity::KMS::Key key;
        key.region = region;
        key.keyId = keyId;
        key.keyUsage = TEST_KEY_USAGE;
        key.keySpec = TEST_KEY_SPEC;
        key.keyState = TEST_KEY_STATE;
        key.origin = TEST_ORIGIN;
        key.description = "test description";
        key.pendingWindowInDays = 7;
        return key;
    }

    struct KmsMemoryDbFixture {
        KmsMemoryDbFixture() {
            RepositoryFactory::instance().initialize(BackendType::MEMORY, "test");
        }

        ~KmsMemoryDbFixture() {
            const long count = RepositoryFactory::instance().kmsRepository()->deleteAllKeys();
            log_debug << "KMS keys deleted, count: " << count;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(KmsMemoryDbTests, KmsMemoryDbFixture)

    BOOST_AUTO_TEST_CASE(KeyCreateTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();
        Entity::KMS::Key key = CreateDefaultKey(TEST_REGION, TEST_KEY_ID);
        repo->createKey(key);

        // act
        const Entity::KMS::Key result = repo->getKeyByKeyId(TEST_KEY_ID);

        // assert
        BOOST_CHECK_EQUAL(result.keyId, TEST_KEY_ID);
        BOOST_CHECK_EQUAL(result.region, TEST_REGION);
        BOOST_CHECK_EQUAL(result.keyUsage, TEST_KEY_USAGE);
        BOOST_CHECK_EQUAL(result.keySpec, TEST_KEY_SPEC);
        BOOST_CHECK(!result.oid.empty());
    }

    BOOST_AUTO_TEST_CASE(KeyExistsTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();
        Entity::KMS::Key key = CreateDefaultKey(TEST_REGION, TEST_KEY_ID);
        repo->createKey(key);

        // act
        const bool result = repo->keyExists(TEST_KEY_ID);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_AUTO_TEST_CASE(KeyNotExistsTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();

        // act
        const bool result = repo->keyExists("nonexistent-key-id");

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_AUTO_TEST_CASE(KeyGetByIdTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();
        Entity::KMS::Key key = CreateDefaultKey(TEST_REGION, TEST_KEY_ID);
        repo->createKey(key);
        const Entity::KMS::Key created = repo->getKeyByKeyId(TEST_KEY_ID);
        BOOST_REQUIRE_EQUAL(created.oid.empty(), false);

        // act
        const Entity::KMS::Key result = repo->getKeyById(created.oid);

        // assert
        BOOST_CHECK_EQUAL(result.keyId, TEST_KEY_ID);
        BOOST_CHECK_EQUAL(result.region, TEST_REGION);
    }

    BOOST_AUTO_TEST_CASE(KeyGetByKeyIdTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();
        Entity::KMS::Key key = CreateDefaultKey(TEST_REGION, TEST_KEY_ID);
        repo->createKey(key);

        // act
        const Entity::KMS::Key result = repo->getKeyByKeyId(TEST_KEY_ID);

        // assert
        BOOST_CHECK_EQUAL(result.keyId, TEST_KEY_ID);
        BOOST_CHECK_EQUAL(result.region, TEST_REGION);
        BOOST_CHECK_EQUAL(result.description, "test description");
    }

    BOOST_AUTO_TEST_CASE(KeyUpdateTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();
        Entity::KMS::Key key = CreateDefaultKey(TEST_REGION, TEST_KEY_ID);
        repo->createKey(key);
        Entity::KMS::Key toUpdate = repo->getKeyByKeyId(TEST_KEY_ID);

        // act
        toUpdate.description = "updated description";
        const Entity::KMS::Key result = repo->updateKey(toUpdate);

        // assert
        BOOST_CHECK_EQUAL(result.keyId, TEST_KEY_ID);
        BOOST_CHECK_EQUAL(result.description, "updated description");
        BOOST_CHECK_EQUAL(repo->getKeyByKeyId(TEST_KEY_ID).description, "updated description");
    }

    BOOST_AUTO_TEST_CASE(KeyUpsertCreateTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();
        Entity::KMS::Key key = CreateDefaultKey(TEST_REGION, TEST_KEY_ID);

        // act
        repo->upsertKey(key);

        // assert
        BOOST_CHECK_EQUAL(repo->countKeys(), 1);
        BOOST_CHECK_EQUAL(repo->getKeyByKeyId(TEST_KEY_ID).keyId, TEST_KEY_ID);
    }

    BOOST_AUTO_TEST_CASE(KeyUpsertUpdateTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();
        Entity::KMS::Key key = CreateDefaultKey(TEST_REGION, TEST_KEY_ID);
        repo->createKey(key);
        Entity::KMS::Key toUpsert = repo->getKeyByKeyId(TEST_KEY_ID);

        // act
        toUpsert.description = "upserted description";
        repo->upsertKey(toUpsert);

        // assert
        BOOST_CHECK_EQUAL(repo->countKeys(), 1);
        BOOST_CHECK_EQUAL(repo->getKeyByKeyId(TEST_KEY_ID).description, "upserted description");
    }

    BOOST_AUTO_TEST_CASE(KeyCountTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();
        Entity::KMS::Key key1 = CreateDefaultKey(TEST_REGION, TEST_KEY_ID);
        repo->createKey(key1);
        Entity::KMS::Key key2 = CreateDefaultKey(TEST_REGION, std::string(TEST_KEY_ID) + "-2");
        repo->createKey(key2);

        // act
        const long count = repo->countKeys();

        // assert
        BOOST_CHECK_EQUAL(count, 2);
    }

    BOOST_AUTO_TEST_CASE(KeyListByRegionTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();
        Entity::KMS::Key key1 = CreateDefaultKey(TEST_REGION, TEST_KEY_ID);
        repo->createKey(key1);
        Entity::KMS::Key key2 = CreateDefaultKey("eu-west-1", std::string(TEST_KEY_ID) + "-2");
        repo->createKey(key2);

        // act
        const Entity::KMS::KeyList allKeys = repo->listKeys("", "", 0, 0, {});
        const Entity::KMS::KeyList regionKeys = repo->listKeys(TEST_REGION, "", 0, 0, {});

        // assert
        BOOST_CHECK_EQUAL(allKeys.size(), 2);
        BOOST_CHECK_EQUAL(regionKeys.size(), 1);
        BOOST_CHECK_EQUAL(regionKeys.front().region, TEST_REGION);
    }

    BOOST_AUTO_TEST_CASE(KeyDeleteTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();
        Entity::KMS::Key key = CreateDefaultKey(TEST_REGION, TEST_KEY_ID);
        repo->createKey(key);

        // act
        repo->deleteKey(key);

        // assert
        BOOST_CHECK_EQUAL(repo->keyExists(TEST_KEY_ID), false);
        BOOST_CHECK_EQUAL(repo->countKeys(), 0);
    }

    BOOST_AUTO_TEST_CASE(KeyDeleteAllTest) {

        // arrange
        const std::shared_ptr<IKMSRepository> repo = RepositoryFactory::instance().kmsRepository();
        Entity::KMS::Key key1 = CreateDefaultKey(TEST_REGION, TEST_KEY_ID);
        repo->createKey(key1);
        Entity::KMS::Key key2 = CreateDefaultKey(TEST_REGION, std::string(TEST_KEY_ID) + "-2");
        repo->createKey(key2);

        // act
        const long count = repo->deleteAllKeys();

        // assert
        BOOST_CHECK_EQUAL(count, 2);
        BOOST_CHECK_EQUAL(repo->countKeys(), 0);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
