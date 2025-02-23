//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_DATABASE_COGNITO_DATABASE_TEST_H
#define AWMOCK_DATABASE_COGNITO_DATABASE_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/CognitoDatabase.h>

#define USER_POOL_ID std::string("test-user-pool_sdjhdjft")
#define USER_POOL_NAME std::string("test-user-pool")
#define USER_NAME "test-user"
#define GROUP_NAME "test-group"
#define CLIENT_ID "asdasjasdkjasd"
#define CLIENT_NAME "test-client"

namespace AwsMock::Database {

    class CognitoDatabaseDbTest : public ::testing::Test {

      protected:

        void SetUp() override {
            _region = _configuration.GetValueString("awsmock.region");
            _accountId = _configuration.GetValueString("awsmock.access.account-id");
        }

        void TearDown() override {
            _cognitoDatabase.DeleteAllUsers();
            _cognitoDatabase.DeleteAllUserPools();
            _cognitoDatabase.DeleteAllGroups();
        }

        std::string _region;
        std::string _accountId;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        CognitoDatabase _cognitoDatabase = CognitoDatabase();
    };

    TEST_F(CognitoDatabaseDbTest, UserPoolCreateTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};

        // act
        const Entity::Cognito::UserPool result = _cognitoDatabase.CreateUserPool(userPool);

        // assert
        EXPECT_TRUE(result.name == USER_POOL_NAME);
        EXPECT_FALSE(result.userPoolId.empty());
    }

    TEST_F(CognitoDatabaseDbTest, UserPoolListTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const Entity::Cognito::UserPoolList result = _cognitoDatabase.ListUserPools(_region);

        // assert
        EXPECT_FALSE(result.empty());
        EXPECT_EQ(1, result.size());
    }

    TEST_F(CognitoDatabaseDbTest, UserPoolExistsTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        const Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const bool result = _cognitoDatabase.UserPoolExists(_region, USER_POOL_NAME);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(CognitoDatabaseDbTest, UserPoolExistsIdTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        const Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const bool result = _cognitoDatabase.UserPoolExists(createUserPoolResult.userPoolId);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(CognitoDatabaseDbTest, UserPoolGetByUserPoolIdTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        const Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const Entity::Cognito::UserPool getUserPool = _cognitoDatabase.GetUserPoolByUserPoolId(createUserPoolResult.userPoolId);

        // assert
        EXPECT_TRUE(getUserPool.userPoolId == USER_POOL_ID);
        EXPECT_TRUE(getUserPool.name == USER_POOL_NAME);
    }

    TEST_F(CognitoDatabaseDbTest, UserPoolGetByClientIdTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        const Entity::Cognito::UserPoolClient userPoolClient = {.userPoolId = USER_POOL_ID, .clientId = CLIENT_ID};
        userPool.userPoolClients.emplace_back(userPoolClient);
        userPool = _cognitoDatabase.UpdateUserPool(userPool);

        // act
        const Entity::Cognito::UserPool getUserPool = _cognitoDatabase.GetUserPoolByClientId(CLIENT_ID);

        // assert
        EXPECT_TRUE(userPool.userPoolId == USER_POOL_ID);
        EXPECT_TRUE(userPool.name == USER_POOL_NAME);
    }

    TEST_F(CognitoDatabaseDbTest, UserPoolGetByRegionNameTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const Entity::Cognito::UserPool getUserPool = _cognitoDatabase.GetUserPoolByRegionName(_region, USER_POOL_NAME);

        // assert
        EXPECT_TRUE(userPool.userPoolId == USER_POOL_ID);
        EXPECT_TRUE(userPool.name == USER_POOL_NAME);
    }

    TEST_F(CognitoDatabaseDbTest, UserPoolUpdateTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        createUserPoolResult.userPoolId = std::string(USER_POOL_NAME) + "2";
        const Entity::Cognito::UserPool updateUserPoolResult = _cognitoDatabase.UpdateUserPool(createUserPoolResult);

        // assert
        EXPECT_TRUE(updateUserPoolResult.userPoolId == std::string(USER_POOL_NAME) + "2");
    }

    TEST_F(CognitoDatabaseDbTest, UserPoolClientFindTest) {

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

    TEST_F(CognitoDatabaseDbTest, UserPoolCountTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        ASSERT_FALSE(userPool.oid.empty());

        // act
        const long count = _cognitoDatabase.CountUserPools(_region);

        // assert
        EXPECT_EQ(1, count);
    }

    TEST_F(CognitoDatabaseDbTest, UserPoolDeleteTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        const Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        _cognitoDatabase.DeleteUserPool(createUserPoolResult.userPoolId);
        const bool result = _cognitoDatabase.UserPoolExists(_region, USER_POOL_NAME);

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(CognitoDatabaseDbTest, UserPoolDeleteAllTest) {

        // arrange
        Entity::Cognito::UserPool userPool1 = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool1 = _cognitoDatabase.CreateUserPool(userPool1);
        ASSERT_FALSE(userPool1.oid.empty());
        Entity::Cognito::UserPool userPool2 = {.region = _region, .userPoolId = USER_POOL_ID + "1", .name = USER_POOL_NAME};
        userPool2 = _cognitoDatabase.CreateUserPool(userPool2);
        ASSERT_FALSE(userPool2.oid.empty());

        // act
        _cognitoDatabase.DeleteAllUserPools();
        const bool result = _cognitoDatabase.UserPoolExists(_region, USER_POOL_NAME);
        const long count = _cognitoDatabase.CountUserPools(_region);

        // assert
        EXPECT_FALSE(result);
        EXPECT_EQ(0, count);
    }

    TEST_F(CognitoDatabaseDbTest, UserExistsTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        const Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        const bool result = _cognitoDatabase.UserExists(createdUser.region, createdUser.userPoolId, createdUser.userName);

        // assert
        EXPECT_TRUE(result);
    }

    TEST_F(CognitoDatabaseDbTest, UserCreateTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};

        // act
        const Entity::Cognito::User result = _cognitoDatabase.CreateUser(user);

        // assert
        EXPECT_TRUE(result.userName == USER_NAME);
        EXPECT_FALSE(result.userPoolId.empty());
        EXPECT_TRUE(result.userPoolId == USER_POOL_ID);
    }

    TEST_F(CognitoDatabaseDbTest, UserGetByOidTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        ASSERT_FALSE(userPool.oid.empty());
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        user = _cognitoDatabase.CreateUser(user);

        // act
        const Entity::Cognito::User result = _cognitoDatabase.GetUserById(bsoncxx::oid(user.oid));

        // assert
        EXPECT_TRUE(result.userName == USER_NAME);
        EXPECT_FALSE(result.userPoolId.empty());
        EXPECT_TRUE(result.userPoolId == USER_POOL_ID);
    }

    TEST_F(CognitoDatabaseDbTest, UserGetByUserNameTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        ASSERT_FALSE(userPool.oid.empty());
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        user = _cognitoDatabase.CreateUser(user);

        // act
        const Entity::Cognito::User result = _cognitoDatabase.GetUserByUserName(userPool.region, userPool.userPoolId, USER_NAME);

        // assert
        EXPECT_TRUE(result.userName == USER_NAME);
        EXPECT_FALSE(result.userPoolId.empty());
        EXPECT_TRUE(result.userPoolId == USER_POOL_ID);
    }

    TEST_F(CognitoDatabaseDbTest, UserGetByIdTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        ASSERT_FALSE(userPool.oid.empty());
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        user = _cognitoDatabase.CreateUser(user);

        // act
        const Entity::Cognito::User result = _cognitoDatabase.GetUserById(user.oid);

        // assert
        EXPECT_TRUE(result.userName == USER_NAME);
        EXPECT_FALSE(result.userPoolId.empty());
        EXPECT_TRUE(result.userPoolId == USER_POOL_ID);
    }

    TEST_F(CognitoDatabaseDbTest, UserCountTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        const long count = _cognitoDatabase.CountUsers();

        // assert
        EXPECT_EQ(1, count);
    }

    TEST_F(CognitoDatabaseDbTest, UserListTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        const Entity::Cognito::UserList userList = _cognitoDatabase.ListUsers();

        // assert
        EXPECT_EQ(1, userList.size());
        EXPECT_TRUE(userList.front().userName == USER_NAME);
    }

    TEST_F(CognitoDatabaseDbTest, UserUpdateTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        user = _cognitoDatabase.CreateUser(user);

        // act
        user.password = "password";
        user = _cognitoDatabase.UpdateUser(user);

        // assert
        EXPECT_TRUE(user.password == "password");
    }

    TEST_F(CognitoDatabaseDbTest, UserDeleteTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user = {.region = _region, .userPoolId = USER_POOL_ID, .userName = USER_NAME};
        Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        _cognitoDatabase.DeleteUser(user);
        const bool result = _cognitoDatabase.UserExists(_region, createdUser.userPoolId, createdUser.userName);

        // assert
        EXPECT_FALSE(result);
    }

    TEST_F(CognitoDatabaseDbTest, UserDeleteAllTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user1 = {.region = _region, .userPoolId = USER_POOL_ID, .userName = std::string(USER_NAME) + "1"};
        user1 = _cognitoDatabase.CreateUser(user1);
        ASSERT_FALSE(user1.oid.empty());
        Entity::Cognito::User user2 = {.region = _region, .userPoolId = USER_POOL_ID, .userName = std::string(USER_NAME) + "2"};
        user2 = _cognitoDatabase.CreateUser(user2);
        ASSERT_FALSE(user2.oid.empty());

        // act
        _cognitoDatabase.DeleteAllUsers();
        const long count = _cognitoDatabase.CountUsers();

        // assert
        EXPECT_EQ(0, count);
    }

    TEST_F(CognitoDatabaseDbTest, GroupExistsTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        ASSERT_FALSE(userPool.oid.empty());
        Entity::Cognito::Group group = {.region = _region, .userPoolId = USER_POOL_ID, .groupName = GROUP_NAME};
        group = _cognitoDatabase.CreateGroup(group);
        ASSERT_FALSE(group.oid.empty());

        // act
        const bool groupExists = _cognitoDatabase.GroupExists(_region, GROUP_NAME);

        // assert
        EXPECT_TRUE(groupExists);
    }

    TEST_F(CognitoDatabaseDbTest, GroupGetByIdTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        ASSERT_FALSE(userPool.oid.empty());
        Entity::Cognito::Group group = {.region = _region, .userPoolId = USER_POOL_ID, .groupName = GROUP_NAME};
        group = _cognitoDatabase.CreateGroup(group);
        ASSERT_FALSE(group.oid.empty());

        // act
        const Entity::Cognito::Group existingGroup = _cognitoDatabase.GetGroupById(bsoncxx::oid(group.oid));

        // assert
        EXPECT_TRUE(group.oid == existingGroup.oid);
    }

    TEST_F(CognitoDatabaseDbTest, GroupGetByNameTest) {

        // arrange
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        ASSERT_FALSE(userPool.oid.empty());
        Entity::Cognito::Group group = {.region = _region, .userPoolId = userPool.userPoolId, .groupName = GROUP_NAME};
        group = _cognitoDatabase.CreateGroup(group);
        ASSERT_FALSE(group.oid.empty());

        // act
        const Entity::Cognito::Group existingGroup = _cognitoDatabase.GetGroupByGroupName(_region, userPool.userPoolId, GROUP_NAME);

        // assert
        EXPECT_TRUE(group.userPoolId == existingGroup.userPoolId);
        EXPECT_TRUE(group.groupName == existingGroup.groupName);
        EXPECT_TRUE(group.region == existingGroup.region);
    }

    TEST_F(CognitoDatabaseDbTest, ClientIdExistsTest) {

        // arrange
        Entity::Cognito::UserPoolClient userPoolClient = {.clientId = "clientId", .clientName = "clientName"};
        Entity::Cognito::UserPool userPool = {.region = _region, .userPoolId = USER_POOL_ID, .name = USER_POOL_NAME};
        userPool.userPoolClients.emplace_back(userPoolClient);
        Entity::Cognito::UserPool result = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const bool clientIdExists = _cognitoDatabase.ClientIdExists(_region, "clientId");

        // assert
        EXPECT_TRUE(clientIdExists);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_DATABASE_COGNITO_DATABASE_TEST_H
