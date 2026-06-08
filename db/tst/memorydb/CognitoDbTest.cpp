//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/repository/cognito/ICognitoRepository.h>

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_USER_POOL_NAME "test-user-pool"
#define TEST_USER_NAME "test-user"
#define TEST_USER_GROUP_NAME "test-user-group"

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

namespace Awsmock::Database {

    Entity::Cognito::UserPool CreateDefaultUserPool(const std::string &region, const std::string &name) {
        Entity::Cognito::UserPool userPool;
        userPool.region = region;
        userPool.name = name;
        userPool.arn = Core::AwsUtils::CreateCognitoUserPoolArn(TEST_REGION, TEST_ACCOUNT_ID, TEST_USER_POOL_NAME);
        return userPool;
    }

    Entity::Cognito::User CreateDefaultUser(const std::string &region, const std::string &userPoolId) {
        Entity::Cognito::User user;
        user.region = region;
        user.userPoolId = userPoolId;
        user.userName = TEST_USER_NAME;
        user.enabled = true;
        return user;
    }

    Entity::Cognito::Group CreateDefaultUserGroup(const std::string &region, const std::string &userPoolId) {
        Entity::Cognito::Group group;
        group.region = region;
        group.userPoolId = userPoolId;
        group.groupName = TEST_USER_GROUP_NAME;
        group.description = "Description";
        return group;
    }

    struct CognitoMemoryDbFixture {
        CognitoMemoryDbFixture() {
            RepositoryFactory::instance().initialize(BackendType::MEMORY, "test");
        }
        ~CognitoMemoryDbFixture() {
            long deleted = RepositoryFactory::instance().cognitoRepository()->deleteAllUsers();
            log_debug << "Users deleted " << deleted;
            deleted = RepositoryFactory::instance().cognitoRepository()->deleteAllGroups({});
            log_debug << "Groups deleted " << deleted;
            deleted = RepositoryFactory::instance().cognitoRepository()->deleteAllUserPools();
            log_debug << "UserPools deleted " << deleted;
        }
    };

    BOOST_FIXTURE_TEST_SUITE(CognitoMemoryDbTests, CognitoMemoryDbFixture)

    BOOST_AUTO_TEST_CASE(createUserPoolTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);

        // act
        userPool = cognitoDatabase->createUserPool(userPool);

        // assert
        BOOST_CHECK_EQUAL(false, userPool.arn.empty());
        BOOST_CHECK_EQUAL(false, userPool.oid.empty());
        BOOST_CHECK_EQUAL(false, userPool.name.empty());
    }

    BOOST_AUTO_TEST_CASE(GetUserPoolByUserPoolIdTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase->createUserPool(userPool);
        BOOST_CHECK_EQUAL(false, userPool.arn.empty());
        BOOST_CHECK_EQUAL(false, userPool.oid.empty());
        BOOST_CHECK_EQUAL(false, userPool.name.empty());

        // act
        const Entity::Cognito::UserPool result = cognitoDatabase->getUserPoolByUserPoolId(userPool.userPoolId);

        // assert
        BOOST_CHECK_EQUAL(userPool.userPoolId, result.userPoolId);
    }

    BOOST_AUTO_TEST_CASE(ExistsByUserPoolIdTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase->createUserPool(userPool);

        // act
        const bool result = cognitoDatabase->userPoolExists(userPool.userPoolId);

        // assert
        BOOST_CHECK_EQUAL(true, result);
    }

    BOOST_AUTO_TEST_CASE(ListUserPoolTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase->createUserPool(userPool);
        BOOST_CHECK_EQUAL(false, userPool.arn.empty());
        BOOST_CHECK_EQUAL(false, userPool.oid.empty());

        // act
        const std::vector<Entity::Cognito::UserPool> userPools = cognitoDatabase->listUserPools({}, {}, 0, 0, {});

        // assert
        BOOST_CHECK_EQUAL(1, userPools.size());
        BOOST_CHECK_EQUAL(false, userPools.at(0).arn.empty());
    }

    BOOST_AUTO_TEST_CASE(CountUserPoolTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase->createUserPool(userPool);
        BOOST_CHECK_EQUAL(false, userPool.arn.empty());
        BOOST_CHECK_EQUAL(false, userPool.oid.empty());

        // act
        const long count = cognitoDatabase->countUserPools({});

        // assert
        BOOST_CHECK_EQUAL(true, count > 0);
    }

    BOOST_AUTO_TEST_CASE(deleteUserPoolTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase->createUserPool(userPool);
        BOOST_CHECK_EQUAL(false, userPool.arn.empty());
        BOOST_CHECK_EQUAL(false, userPool.oid.empty());

        // act
        cognitoDatabase->deleteUserPool(userPool.userPoolId);
        const long count = cognitoDatabase->countUserPools({});

        // assert
        BOOST_CHECK_EQUAL(0, count);
    }

    BOOST_AUTO_TEST_CASE(createUserTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::User user = CreateDefaultUser(TEST_REGION, userPool.userPoolId);

        // act
        user = cognitoDatabase->createUser(user);

        // assert
        BOOST_CHECK_EQUAL(false, user.oid.empty());
        BOOST_CHECK_EQUAL(false, user.userName.empty());
    }

    BOOST_AUTO_TEST_CASE(ExistsUserTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::User user = CreateDefaultUser(TEST_REGION, userPool.userPoolId);
        user = cognitoDatabase->createUser(user);
        BOOST_CHECK_EQUAL(false, user.oid.empty());
        BOOST_CHECK_EQUAL(false, user.userName.empty());

        // act
        const bool result = cognitoDatabase->userExists(user.region, user.userName);

        // assert
        BOOST_CHECK_EQUAL(true, result);
    }

    BOOST_AUTO_TEST_CASE(CountUserTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::User user = CreateDefaultUser(TEST_REGION, userPool.userPoolId);
        user = cognitoDatabase->createUser(user);
        BOOST_CHECK_EQUAL(false, user.oid.empty());
        BOOST_CHECK_EQUAL(false, user.userName.empty());

        // act
        const long count = cognitoDatabase->countUsers({}, {}, {});

        // assert
        BOOST_CHECK_EQUAL(1, count);
    }

    BOOST_AUTO_TEST_CASE(ListUserTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::User user = CreateDefaultUser(TEST_REGION, userPool.userPoolId);
        user = cognitoDatabase->createUser(user);
        BOOST_CHECK_EQUAL(false, user.oid.empty());
        BOOST_CHECK_EQUAL(false, user.userName.empty());

        // act
        const std::vector<Entity::Cognito::User> users = cognitoDatabase->listUsers({}, {}, {}, 0, 0, {});

        // assert
        BOOST_CHECK_EQUAL(1, users.size());
    }

    BOOST_AUTO_TEST_CASE(DeleteUserTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::User user = CreateDefaultUser(TEST_REGION, userPool.userPoolId);
        user = cognitoDatabase->createUser(user);
        BOOST_CHECK_EQUAL(false, user.oid.empty());
        BOOST_CHECK_EQUAL(false, user.userName.empty());

        // act
        const long deleted = cognitoDatabase->deleteUser(user);
        const long count = cognitoDatabase->countUsers({}, {}, {});

        // assert
        BOOST_CHECK_EQUAL(1, deleted);
        BOOST_CHECK_EQUAL(0, count);
    }

    BOOST_AUTO_TEST_CASE(createUserGroupTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::Group userGroup = CreateDefaultUserGroup(TEST_REGION, userPool.userPoolId);

        // act
        userGroup = cognitoDatabase->createGroup(userGroup);

        // assert
        BOOST_CHECK_EQUAL(false, userGroup.oid.empty());
        BOOST_CHECK_EQUAL(false, userGroup.groupName.empty());
    }

    BOOST_AUTO_TEST_CASE(ExistsUserGroupTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::Group userGroup = CreateDefaultUserGroup(TEST_REGION, userPool.userPoolId);
        userGroup = cognitoDatabase->createGroup(userGroup);

        // act
        const bool result = cognitoDatabase->groupExists(userGroup.region, userGroup.groupName);

        // assert
        BOOST_CHECK_EQUAL(true, result);
    }

    BOOST_AUTO_TEST_CASE(ListUserGroupTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::Group userGroup = CreateDefaultUserGroup(TEST_REGION, userPool.userPoolId);
        userGroup = cognitoDatabase->createGroup(userGroup);
        BOOST_CHECK_EQUAL(false, userGroup.oid.empty());
        BOOST_CHECK_EQUAL(false, userGroup.groupName.empty());

        // act
        const std::vector<Entity::Cognito::Group> userGroups = cognitoDatabase->listGroups({}, {});

        // assert
        BOOST_CHECK_EQUAL(1, userGroups.size());
    }

    BOOST_AUTO_TEST_CASE(DeleteUserGroupTest) {

        // arrange
        const std::shared_ptr<ICognitoRepository> cognitoDatabase = RepositoryFactory::instance().cognitoRepository();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::Group userGroup = CreateDefaultUserGroup(TEST_REGION, userPool.userPoolId);
        userGroup = cognitoDatabase->createGroup(userGroup);
        BOOST_CHECK_EQUAL(false, userGroup.oid.empty());
        BOOST_CHECK_EQUAL(false, userGroup.groupName.empty());

        // act
        const long count = cognitoDatabase->deleteGroup(userGroup.region, userGroup.userPoolId, userGroup.groupName);

        // assert
        BOOST_CHECK_EQUAL(1, count);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database