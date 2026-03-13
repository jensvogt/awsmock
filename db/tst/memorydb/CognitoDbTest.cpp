//
// Created by vogje01 on 02/06/2023.
//
// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/repository/CognitoDatabase.h>

#define TEST_ACCOUNT_ID "000000000000"
#define TEST_REGION "eu-central-1"
#define TEST_USER_POOL_NAME "test-user-pool"
#define TEST_USER_NAME "test-user"
#define TEST_USER_GROUP_NAME "test-user-group"

namespace AwsMock::Database {

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
        CognitoMemoryDbFixture() = default;
        ~CognitoMemoryDbFixture() {
            CognitoDatabase::instance().DeleteAllUsers();
            CognitoDatabase::instance().DeleteAllGroups();
            CognitoDatabase::instance().DeleteAllUserPools();
        }
    };

    BOOST_FIXTURE_TEST_SUITE(CognitoMemoryDbTests, CognitoMemoryDbFixture)

    BOOST_AUTO_TEST_CASE(CreateUserPoolTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);

        // act
        userPool = cognitoDatabase.CreateUserPool(userPool);

        // assert
        BOOST_CHECK_EQUAL(false, userPool.arn.empty());
        BOOST_CHECK_EQUAL(false, userPool.oid.empty());
        BOOST_CHECK_EQUAL(false, userPool.name.empty());
    }

    BOOST_AUTO_TEST_CASE(GetUserPoolByUserPoolIdTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase.CreateUserPool(userPool);
        BOOST_CHECK_EQUAL(false, userPool.arn.empty());
        BOOST_CHECK_EQUAL(false, userPool.oid.empty());
        BOOST_CHECK_EQUAL(false, userPool.name.empty());

        // act
        Entity::Cognito::UserPool result = cognitoDatabase.GetUserPoolByUserPoolId(userPool.userPoolId);

        // assert
        BOOST_CHECK_EQUAL(userPool.userPoolId, result.userPoolId);
    }

    BOOST_AUTO_TEST_CASE(ExistsByUserPoolIdTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase.CreateUserPool(userPool);

        // act
        const bool result = cognitoDatabase.UserPoolExists(userPool.userPoolId);

        // assert
        BOOST_CHECK_EQUAL(true, result);
    }

    BOOST_AUTO_TEST_CASE(ListUserPoolTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase.CreateUserPool(userPool);
        BOOST_CHECK_EQUAL(false, userPool.arn.empty());
        BOOST_CHECK_EQUAL(false, userPool.oid.empty());

        // act
        const std::vector<Entity::Cognito::UserPool> userPools = cognitoDatabase.ListUserPools();

        // assert
        BOOST_CHECK_EQUAL(1, userPools.size());
        BOOST_CHECK_EQUAL(false, userPools.at(0).arn.empty());
    }

    BOOST_AUTO_TEST_CASE(CountUserPoolTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase.CreateUserPool(userPool);
        BOOST_CHECK_EQUAL(false, userPool.arn.empty());
        BOOST_CHECK_EQUAL(false, userPool.oid.empty());

        // act
        const long count = cognitoDatabase.CountUserPools();

        // assert
        BOOST_CHECK_EQUAL(true, count > 0);
    }

    BOOST_AUTO_TEST_CASE(DeleteUserPoolTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase.CreateUserPool(userPool);
        BOOST_CHECK_EQUAL(false, userPool.arn.empty());
        BOOST_CHECK_EQUAL(false, userPool.oid.empty());

        // act
        cognitoDatabase.DeleteUserPool(userPool.userPoolId);
        const long count = cognitoDatabase.CountUserPools();

        // assert
        BOOST_CHECK_EQUAL(0, count);
    }

    BOOST_AUTO_TEST_CASE(CreateUserTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::User user = CreateDefaultUser(TEST_REGION, userPool.userPoolId);

        // act
        user = cognitoDatabase.CreateUser(user);

        // assert
        BOOST_CHECK_EQUAL(false, user.oid.empty());
        BOOST_CHECK_EQUAL(false, user.userName.empty());
    }

    BOOST_AUTO_TEST_CASE(ExistsUserTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::User user = CreateDefaultUser(TEST_REGION, userPool.userPoolId);
        user = cognitoDatabase.CreateUser(user);
        BOOST_CHECK_EQUAL(false, user.oid.empty());
        BOOST_CHECK_EQUAL(false, user.userName.empty());

        // act
        const bool result = cognitoDatabase.UserExists(user.region, user.userName);

        // assert
        BOOST_CHECK_EQUAL(true, result);
    }

    BOOST_AUTO_TEST_CASE(CountUserTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::User user = CreateDefaultUser(TEST_REGION, userPool.userPoolId);
        user = cognitoDatabase.CreateUser(user);
        BOOST_CHECK_EQUAL(false, user.oid.empty());
        BOOST_CHECK_EQUAL(false, user.userName.empty());

        // act
        const long count = cognitoDatabase.CountUsers();

        // assert
        BOOST_CHECK_EQUAL(1, count);
    }

    BOOST_AUTO_TEST_CASE(ListUserTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::User user = CreateDefaultUser(TEST_REGION, userPool.userPoolId);
        user = cognitoDatabase.CreateUser(user);
        BOOST_CHECK_EQUAL(false, user.oid.empty());
        BOOST_CHECK_EQUAL(false, user.userName.empty());

        // act
        const std::vector<Entity::Cognito::User> users = cognitoDatabase.ListUsers();

        // assert
        BOOST_CHECK_EQUAL(1, users.size());
    }

    BOOST_AUTO_TEST_CASE(DeleteUserTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::User user = CreateDefaultUser(TEST_REGION, userPool.userPoolId);
        user = cognitoDatabase.CreateUser(user);
        BOOST_CHECK_EQUAL(false, user.oid.empty());
        BOOST_CHECK_EQUAL(false, user.userName.empty());

        // act
        const long deleted = cognitoDatabase.DeleteUser(user);
        const long count = cognitoDatabase.CountUsers();

        // assert
        BOOST_CHECK_EQUAL(1, deleted);
        BOOST_CHECK_EQUAL(0, count);
    }

    BOOST_AUTO_TEST_CASE(CreateUserGroupTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::Group userGroup = CreateDefaultUserGroup(TEST_REGION, userPool.userPoolId);

        // act
        userGroup = cognitoDatabase.CreateGroup(userGroup);

        // assert
        BOOST_CHECK_EQUAL(false, userGroup.oid.empty());
        BOOST_CHECK_EQUAL(false, userGroup.groupName.empty());
    }

    BOOST_AUTO_TEST_CASE(ExistsUserGroupTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::Group userGroup = CreateDefaultUserGroup(TEST_REGION, userPool.userPoolId);
        userGroup = cognitoDatabase.CreateGroup(userGroup);

        // act
        const bool result = cognitoDatabase.GroupExists(userGroup.region, userGroup.groupName);

        // assert
        BOOST_CHECK_EQUAL(true, result);
    }

    BOOST_AUTO_TEST_CASE(ListUserGroupTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::Group userGroup = CreateDefaultUserGroup(TEST_REGION, userPool.userPoolId);
        userGroup = cognitoDatabase.CreateGroup(userGroup);
        BOOST_CHECK_EQUAL(false, userGroup.oid.empty());
        BOOST_CHECK_EQUAL(false, userGroup.groupName.empty());

        // act
        const std::vector<Entity::Cognito::Group> userGroups = cognitoDatabase.ListGroups();

        // assert
        BOOST_CHECK_EQUAL(1, userGroups.size());
    }

    BOOST_AUTO_TEST_CASE(DeleteUserGroupTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        const Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        Entity::Cognito::Group userGroup = CreateDefaultUserGroup(TEST_REGION, userPool.userPoolId);
        userGroup = cognitoDatabase.CreateGroup(userGroup);
        BOOST_CHECK_EQUAL(false, userGroup.oid.empty());
        BOOST_CHECK_EQUAL(false, userGroup.groupName.empty());

        // act
        const long count = cognitoDatabase.DeleteGroup(userGroup.region, userGroup.userPoolId, userGroup.groupName);

        // assert
        BOOST_CHECK_EQUAL(1, count);
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace AwsMock::Database