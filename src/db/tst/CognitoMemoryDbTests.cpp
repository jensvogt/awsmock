//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_COGNITOMEMORYDBTEST_H
#define AWMOCK_CORE_COGNITOMEMORYDBTEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/CognitoDatabase.h>

#define USER_POOL_ID "test-user-pool_sdjhdjft"
#define USER_POOL_NAME "test-user-pool"
#define USER_NAME "test-user"
#define CLIENT_ID "asdasjasdkjasd"
#define CLIENT_NAME "test-client"

namespace AwsMock::Database {

    class CognitoMemoryDbTest : public ::testing::Test {

      protected:

        void SetUp() override {
            _region = _configuration.GetValue<std::string>("awsmock.region");
            _accountId = _configuration.GetValue<std::string>("awsmock.access.account-id");
        }

        void TearDown() override {
            _cognitoDatabase.DeleteAllUsers();
            _cognitoDatabase.DeleteAllUserPools();
            _cognitoDatabase.DeleteAllGroups();
        }

        std::string _region;
        std::string _accountId;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration(false);
        CognitoDatabase _cognitoDatabase = CognitoDatabase();
    };

    TEST_F(CognitoMemoryDbTest, UserPoolCreateTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};

        // act
        const Entity::Cognito::UserPool result = _cognitoDatabase.CreateUserPool(userPool);

        // assert
        EXPECT_TRUE(result.name == USER_POOL_NAME);
        EXPECT_FALSE(result.userPoolId.empty());
    }

    TEST_F(CognitoMemoryDbTest, ListUserPoolsTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const Entity::Cognito::UserPoolList result = _cognitoDatabase.ListUserPools(_region);

        // assert
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(1, result.size());
    }

    TEST_F(CognitoMemoryDbTest, UserPoolExistsTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const bool result = _cognitoDatabase.UserPoolExists(_region, USER_POOL_NAME);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(CognitoMemoryDbTest, UserPoolUpdateTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        createUserPoolResult.userPoolId = std::string(USER_POOL_NAME) + "2";
        const Entity::Cognito::UserPool updateUserPoolResult = _cognitoDatabase.UpdateUserPool(createUserPoolResult);

        // assert
        EXPECT_TRUE(updateUserPoolResult.userPoolId == std::string(USER_POOL_NAME) + "2");
    }

    TEST_F(CognitoMemoryDbTest, UserPoolClientFindTest) {

        // arrange
        Entity::Cognito::UserPoolClient userPoolClient = {.userPoolId = USER_POOL_ID, .clientId = CLIENT_ID, .clientName = CLIENT_NAME};
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool.userPoolClients.emplace_back(userPoolClient);
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const Entity::Cognito::UserPool resultUserPool = _cognitoDatabase.GetUserPoolByClientId(CLIENT_ID);

        // assert
        EXPECT_TRUE(resultUserPool.userPoolId == USER_POOL_ID);
    }

    TEST_F(CognitoMemoryDbTest, DeleteUserPoolTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        const Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        _cognitoDatabase.DeleteUserPool(createUserPoolResult.userPoolId);
        const bool result = _cognitoDatabase.UserPoolExists(_region, USER_POOL_NAME);

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(CognitoMemoryDbTest, UserCreateTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};

        // act
        Entity::Cognito::User result = _cognitoDatabase.CreateUser(user);

        // assert
        EXPECT_TRUE(result.userName == USER_NAME);
        EXPECT_FALSE(result.userPoolId.empty());
        EXPECT_TRUE(result.userPoolId == USER_POOL_ID);
    }

    TEST_F(CognitoMemoryDbTest, UserExistsTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        bool result = _cognitoDatabase.UserExists(createdUser.region, createdUser.userPoolId, createdUser.userName);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(CognitoMemoryDbTest, UserCountTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        long count = _cognitoDatabase.CountUsers();

        // assert
        EXPECT_EQ(1, count);
    }

    TEST_F(CognitoMemoryDbTest, UserListTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        Entity::Cognito::UserList userList = _cognitoDatabase.ListUsers();

        // assert
        EXPECT_EQ(1, userList.size());
        EXPECT_TRUE(userList.front().userName == USER_NAME);
    }

    TEST_F(CognitoMemoryDbTest, UserDeleteTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        _cognitoDatabase.DeleteUser(createdUser);
        long count = _cognitoDatabase.CountUsers();

        // assert
        EXPECT_EQ(0, count);
    }

    TEST_F(CognitoMemoryDbTest, ClientIdExistsTest) {

        // arrange
        Entity::Cognito::UserPoolClient userPoolClient = {.clientId = "clientId", .clientName = "clientName"};
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool.userPoolClients.emplace_back(userPoolClient);
        Entity::Cognito::UserPool result = _cognitoDatabase.CreateUserPool(userPool);

        // act
        bool clientIdExists = _cognitoDatabase.ClientIdExists(_region, "clientId");

        // assert
        EXPECT_TRUE(clientIdExists);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_CORE_COGNITOMEMORYDBTEST_H