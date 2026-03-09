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

    struct CognitoMemoryDbFixture {
        CognitoMemoryDbFixture() {
        }
        ~CognitoMemoryDbFixture() {
            CognitoDatabase::instance().DeleteAllUsers();
            CognitoDatabase::instance().DeleteAllUserPools();
        }
    };

    BOOST_FIXTURE_TEST_SUITE(CognitoMemoryDbTests, CognitoMemoryDbFixture)

    BOOST_AUTO_TEST_CASE(CreateUserpoolTest) {

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

    BOOST_AUTO_TEST_CASE(ListUserpoolTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase.CreateUserPool(userPool);

        // act
        const std::vector<Entity::Cognito::UserPool> userPools = cognitoDatabase.ListUserPools();

        // assert
        BOOST_CHECK_EQUAL(true, userPools.size() > 0);
        BOOST_CHECK_EQUAL(false, userPools.at(0).arn.empty());
    }

    BOOST_AUTO_TEST_CASE(CountUserpoolTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase.CreateUserPool(userPool);

        // act
        const long count = cognitoDatabase.CountUserPools();

        // assert
        BOOST_CHECK_EQUAL(true, count > 0);
    }

    BOOST_AUTO_TEST_CASE(DeleteUserpoolTest) {

        // arrange
        const CognitoDatabase &cognitoDatabase = CognitoDatabase::instance();
        Entity::Cognito::UserPool userPool = CreateDefaultUserPool(TEST_REGION, TEST_USER_POOL_NAME);
        userPool = cognitoDatabase.CreateUserPool(userPool);

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

    BOOST_AUTO_TEST_SUITE_END()

}// namespace AwsMock::Database