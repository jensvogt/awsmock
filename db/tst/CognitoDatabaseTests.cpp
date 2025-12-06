//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_DATABASE_COGNITO_DATABASE_TEST_H
#define AWMOCK_DATABASE_COGNITO_DATABASE_TEST_H

// Boost includes
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>

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

    struct CognitoDatabaseDbTest {

        CognitoDatabaseDbTest() {
            _region = _configuration.GetValue<std::string>("awsmock.region");
            _accountId = _configuration.GetValue<std::string>("awsmock.access.account-id");
        }

        ~CognitoDatabaseDbTest() {
            _cognitoDatabase.DeleteAllUsers();
            _cognitoDatabase.DeleteAllUserPools();
            _cognitoDatabase.DeleteAllGroups();
        }

        std::string _region;
        std::string _accountId;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        CognitoDatabase _cognitoDatabase = CognitoDatabase();
    };

    BOOST_FIXTURE_TEST_CASE(UserPoolCreateTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;

        // act
        const Entity::Cognito::UserPool result = _cognitoDatabase.CreateUserPool(userPool);

        // assert
        BOOST_CHECK_EQUAL(result.name, USER_POOL_NAME);
        BOOST_CHECK_EQUAL(result.userPoolId.empty(), false);
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolListTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const Entity::Cognito::UserPoolList result = _cognitoDatabase.ListUserPools(_region);

        // assert
        BOOST_CHECK_EQUAL(result.empty(), false);
        BOOST_CHECK_EQUAL(1, result.size());
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolExistsTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        const Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const bool result = _cognitoDatabase.UserPoolExists(_region, USER_POOL_NAME);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolExistsIdTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        const Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const bool result = _cognitoDatabase.UserPoolExists(createUserPoolResult.userPoolId);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolGetByUserPoolIdTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        const Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const Entity::Cognito::UserPool getUserPool = _cognitoDatabase.GetUserPoolByUserPoolId(createUserPoolResult.userPoolId);

        // assert
        BOOST_CHECK_EQUAL(getUserPool.userPoolId, USER_POOL_ID);
        BOOST_CHECK_EQUAL(getUserPool.name, USER_POOL_NAME);
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolGetByClientIdTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::UserPoolClient userPoolClient;
        userPoolClient.userPoolId = USER_POOL_ID;
        userPoolClient.clientId = CLIENT_ID;
        userPool.userPoolClients.emplace_back(userPoolClient);
        userPool = _cognitoDatabase.UpdateUserPool(userPool);

        // act
        const Entity::Cognito::UserPool getUserPool = _cognitoDatabase.GetUserPoolByClientId(CLIENT_ID);

        // assert
        BOOST_CHECK_EQUAL(userPool.userPoolId, USER_POOL_ID);
        BOOST_CHECK_EQUAL(userPool.name, USER_POOL_NAME);
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolGetByRegionNameTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        userPool = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const Entity::Cognito::UserPool getUserPool = _cognitoDatabase.GetUserPoolByRegionName(_region, USER_POOL_NAME);

        // assert
        BOOST_CHECK_EQUAL(userPool.userPoolId, USER_POOL_ID);
        BOOST_CHECK_EQUAL(userPool.name, USER_POOL_NAME);
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolUpdateTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        createUserPoolResult.userPoolId = std::string(USER_POOL_NAME) + "2";
        const Entity::Cognito::UserPool updateUserPoolResult = _cognitoDatabase.UpdateUserPool(createUserPoolResult);

        // assert
        BOOST_CHECK_EQUAL(updateUserPoolResult.userPoolId, std::string(USER_POOL_NAME) + "2");
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolClientFindTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        Entity::Cognito::UserPoolClient userPoolClient;
        userPoolClient.userPoolId = USER_POOL_ID;
        userPoolClient.clientId = CLIENT_ID;
        userPoolClient.clientName = CLIENT_NAME;
        userPool.userPoolClients.emplace_back(userPoolClient);
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const Entity::Cognito::UserPool resultUserPool = _cognitoDatabase.GetUserPoolByClientId(CLIENT_ID);

        // assert
        BOOST_CHECK_EQUAL(resultUserPool.userPoolId, USER_POOL_ID);
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolCountTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        BOOST_CHECK_EQUAL(userPool.oid.empty(), false);

        // act
        const long count = _cognitoDatabase.CountUserPools(_region);

        // assert
        BOOST_CHECK_EQUAL(1, count);
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolDeleteTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        const Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        _cognitoDatabase.DeleteUserPool(createUserPoolResult.userPoolId);
        const bool result = _cognitoDatabase.UserPoolExists(_region, USER_POOL_NAME);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolDeleteAllTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool1;
        userPool1.region = _region;
        userPool1.userPoolId = USER_POOL_ID;
        userPool1.name = USER_POOL_NAME;
        userPool1 = _cognitoDatabase.CreateUserPool(userPool1);
        BOOST_CHECK_EQUAL(userPool1.oid.empty(), false);
        Entity::Cognito::UserPool userPool2;
        userPool2.region = _region;
        userPool2.userPoolId = USER_POOL_ID;
        userPool2.name = USER_POOL_NAME;
        userPool2 = _cognitoDatabase.CreateUserPool(userPool2);
        BOOST_CHECK_EQUAL(userPool2.oid.empty(), false);

        // act
        _cognitoDatabase.DeleteAllUserPools();
        const bool result = _cognitoDatabase.UserPoolExists(_region, USER_POOL_NAME);
        const long count = _cognitoDatabase.CountUserPools(_region);

        // assert
        BOOST_CHECK_EQUAL(result, false);
        BOOST_CHECK_EQUAL(0, count);
    }

    BOOST_FIXTURE_TEST_CASE(UserExistsTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        const Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user;
        user.region = _region;
        user.userPoolId = createUserPoolResult.userPoolId;
        user.userName = USER_NAME;
        const Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        const bool result = _cognitoDatabase.UserExists(createdUser.region, createdUser.userPoolId, createdUser.userName);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(UserCreateTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user;
        user.region = _region;
        user.userPoolId = USER_POOL_ID;
        user.userName = USER_NAME;

        // act
        const Entity::Cognito::User result = _cognitoDatabase.CreateUser(user);

        // assert
        BOOST_CHECK_EQUAL(result.userName, USER_NAME);
        BOOST_CHECK_EQUAL(result.userPoolId.empty(), false);
        BOOST_CHECK_EQUAL(result.userPoolId, USER_POOL_ID);
    }

    BOOST_FIXTURE_TEST_CASE(UserGetByOidTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        BOOST_CHECK_EQUAL(userPool.oid.empty(), false);
        Entity::Cognito::User user;
        user.region = _region;
        user.userPoolId = USER_POOL_ID;
        user.userName = USER_NAME;
        user = _cognitoDatabase.CreateUser(user);

        // act
        const Entity::Cognito::User result = _cognitoDatabase.GetUserById(bsoncxx::oid(user.oid));

        // assert
        BOOST_CHECK_EQUAL(result.userName, USER_NAME);
        BOOST_CHECK_EQUAL(result.userPoolId.empty(), false);
        BOOST_CHECK_EQUAL(result.userPoolId, USER_POOL_ID);
    }

    BOOST_FIXTURE_TEST_CASE(UserGetByUserNameTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        BOOST_CHECK_EQUAL(userPool.oid.empty(), false);
        Entity::Cognito::User user;
        user.region = _region;
        user.userPoolId = USER_POOL_ID;
        user.userName = USER_NAME;
        user = _cognitoDatabase.CreateUser(user);

        // act
        const Entity::Cognito::User result = _cognitoDatabase.GetUserByUserName(userPool.region, userPool.userPoolId, USER_NAME);

        // assert
        BOOST_CHECK_EQUAL(result.userName, USER_NAME);
        BOOST_CHECK_EQUAL(result.userPoolId.empty(), false);
        BOOST_CHECK_EQUAL(result.userPoolId, USER_POOL_ID);
    }

    BOOST_FIXTURE_TEST_CASE(UserGetByIdTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        BOOST_CHECK_EQUAL(userPool.oid.empty(), false);
        Entity::Cognito::User user;
        user.region = _region;
        user.userPoolId = USER_POOL_ID;
        user.userName = USER_NAME;
        user = _cognitoDatabase.CreateUser(user);

        // act
        const Entity::Cognito::User result = _cognitoDatabase.GetUserById(user.oid);

        // assert
        BOOST_CHECK_EQUAL(result.userName, USER_NAME);
        BOOST_CHECK_EQUAL(result.userPoolId.empty(), false);
        BOOST_CHECK_EQUAL(result.userPoolId, USER_POOL_ID);
    }

    BOOST_FIXTURE_TEST_CASE(UserCountTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user;
        user.region = _region;
        user.userPoolId = USER_POOL_ID;
        user.userName = USER_NAME;
        Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        const long count = _cognitoDatabase.CountUsers();

        // assert
        BOOST_CHECK_EQUAL(1, count);
    }

    BOOST_FIXTURE_TEST_CASE(UserListTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user;
        user.region = _region;
        user.userPoolId = USER_POOL_ID;
        user.userName = USER_NAME;
        Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        const Entity::Cognito::UserList userList = _cognitoDatabase.ListUsers();

        // assert
        BOOST_CHECK_EQUAL(1, userList.size());
        BOOST_CHECK_EQUAL(userList.front().userName, USER_NAME);
    }

    BOOST_FIXTURE_TEST_CASE(UserUpdateTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user;
        user.region = _region;
        user.userPoolId = USER_POOL_ID;
        user.userName = USER_NAME;
        user = _cognitoDatabase.CreateUser(user);

        // act
        user.password = "password";
        user = _cognitoDatabase.UpdateUser(user);

        // assert
        BOOST_CHECK_EQUAL(user.password, "password");
    }

    BOOST_FIXTURE_TEST_CASE(UserDeleteTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user;
        user.region = _region;
        user.userPoolId = USER_POOL_ID;
        user.userName = USER_NAME;
        Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        _cognitoDatabase.DeleteUser(user);
        const bool result = _cognitoDatabase.UserExists(_region, createdUser.userPoolId, createdUser.userName);

        // assert
        BOOST_CHECK_EQUAL(result, false);
    }

    BOOST_FIXTURE_TEST_CASE(UserDeleteAllTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);
        Entity::Cognito::User user1;
        user1.region = _region;
        user1.userPoolId = USER_POOL_ID;
        user1.userName = std::string(USER_NAME) + "1";
        user1 = _cognitoDatabase.CreateUser(user1);
        BOOST_CHECK_EQUAL(user1.oid.empty(), false);
        Entity::Cognito::User user2;
        user2.region = _region;
        user2.userPoolId = USER_POOL_ID;
        user2.userName = std::string(USER_NAME) + "2";
        user2 = _cognitoDatabase.CreateUser(user2);
        BOOST_CHECK_EQUAL(user2.oid.empty(), false);

        // act
        _cognitoDatabase.DeleteAllUsers();
        const long count = _cognitoDatabase.CountUsers();

        // assert
        BOOST_CHECK_EQUAL(0, count);
    }
    BOOST_FIXTURE_TEST_CASE(GroupExistsTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        BOOST_CHECK_EQUAL(userPool.oid.empty(), false);
        Entity::Cognito::Group group;
        group.region = _region;
        group.userPoolId = USER_POOL_ID;
        group.groupName = GROUP_NAME;
        group = _cognitoDatabase.CreateGroup(group);
        BOOST_CHECK_EQUAL(group.oid.empty(), false);

        // act
        const bool groupExists = _cognitoDatabase.GroupExists(_region, GROUP_NAME);

        // assert
        BOOST_CHECK_EQUAL(groupExists, true);
    }

    BOOST_FIXTURE_TEST_CASE(GroupGetByIdTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        BOOST_CHECK_EQUAL(userPool.oid.empty(), false);
        Entity::Cognito::Group group;
        group.region = _region;
        group.userPoolId = USER_POOL_ID;
        group.groupName = GROUP_NAME;
        group = _cognitoDatabase.CreateGroup(group);
        BOOST_CHECK_EQUAL(group.oid.empty(), false);

        // act
        const Entity::Cognito::Group existingGroup = _cognitoDatabase.GetGroupById(bsoncxx::oid(group.oid));

        // assert
        BOOST_CHECK_EQUAL(group.oid, existingGroup.oid);
    }

    BOOST_FIXTURE_TEST_CASE(GroupGetByNameTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        userPool = _cognitoDatabase.CreateUserPool(userPool);
        BOOST_CHECK_EQUAL(userPool.oid.empty(), false);
        Entity::Cognito::Group group;
        group.region = _region;
        group.userPoolId = userPool.userPoolId;
        group.groupName = GROUP_NAME;
        group = _cognitoDatabase.CreateGroup(group);
        BOOST_CHECK_EQUAL(group.oid.empty(), false);

        // act
        const Entity::Cognito::Group existingGroup = _cognitoDatabase.GetGroupByGroupName(_region, userPool.userPoolId, GROUP_NAME);

        // assert
        BOOST_CHECK_EQUAL(group.userPoolId, existingGroup.userPoolId);
        BOOST_CHECK_EQUAL(group.groupName, existingGroup.groupName);
        BOOST_CHECK_EQUAL(group.region, existingGroup.region);
    }

    BOOST_FIXTURE_TEST_CASE(ClientIdExistsTest, CognitoDatabaseDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        Entity::Cognito::UserPoolClient userPoolClient;
        userPoolClient.clientId = "clientId";
        userPoolClient.clientName = "clientName";
        userPool.userPoolClients.emplace_back(userPoolClient);
        Entity::Cognito::UserPool result = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const bool clientIdExists = _cognitoDatabase.ClientIdExists(_region, "clientId");

        // assert
        BOOST_CHECK_EQUAL(clientIdExists, true);
    }

}// namespace AwsMock::Database

#endif// AWMOCK_DATABASE_COGNITO_DATABASE_TEST_H
