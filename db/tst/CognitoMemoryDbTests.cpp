//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_CORE_COGNITO_MEMORYDB_TEST_H
#define AWMOCK_CORE_COGNITO_MEMORYDB_TEST_H

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/CognitoDatabase.h>

#define USER_NAME "test-user"
#define CLIENT_ID "asdasjasdkjasd"
#define CLIENT_NAME "test-client"

namespace AwsMock::Database::MemoryDb {

    struct CognitoMemoryDbTest {

        CognitoMemoryDbTest() {
            _region = _configuration.GetValue<std::string>("awsmock.region");
            _accountId = _configuration.GetValue<std::string>("awsmock.access.account-id");
        }

        ~CognitoMemoryDbTest() {
            _cognitoDatabase.DeleteAllUsers();
            _cognitoDatabase.DeleteAllUserPools();
            _cognitoDatabase.DeleteAllGroups();
        }

        std::string _region;
        std::string _accountId;
        Core::Configuration &_configuration = Core::TestUtils::GetTestConfiguration();
        CognitoDatabase _cognitoDatabase = CognitoDatabase();
    };

    BOOST_FIXTURE_TEST_CASE(UserPoolCreateMTest, CognitoMemoryDbTest) {

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

    BOOST_FIXTURE_TEST_CASE(UserPoolListMTest, CognitoMemoryDbTest) {

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

    BOOST_FIXTURE_TEST_CASE(UserPoolExistsMTest, CognitoMemoryDbTest) {

        // arrange
        Entity::Cognito::UserPool userPool;
        userPool.region = _region;
        userPool.userPoolId = USER_POOL_ID;
        userPool.name = USER_POOL_NAME;
        Entity::Cognito::UserPool createUserPoolResult = _cognitoDatabase.CreateUserPool(userPool);

        // act
        const bool result = _cognitoDatabase.UserPoolExists(_region, USER_POOL_NAME);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(UserPoolUpdateMTest, CognitoMemoryDbTest) {

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

    BOOST_FIXTURE_TEST_CASE(UserPoolClientFindMTest, CognitoMemoryDbTest) {

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

    BOOST_FIXTURE_TEST_CASE(DeleteUserMPoolTest, CognitoMemoryDbTest) {

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

    BOOST_FIXTURE_TEST_CASE(UserCreateMTest, CognitoMemoryDbTest) {

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

    BOOST_FIXTURE_TEST_CASE(UserExistsMTest, CognitoMemoryDbTest) {

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
        const Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        const bool result = _cognitoDatabase.UserExists(createdUser.region, createdUser.userPoolId, createdUser.userName);

        // assert
        BOOST_CHECK_EQUAL(result, true);
    }

    BOOST_FIXTURE_TEST_CASE(UserCountMTest, CognitoMemoryDbTest) {

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

    BOOST_FIXTURE_TEST_CASE(UserListMTest, CognitoMemoryDbTest) {

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

    BOOST_FIXTURE_TEST_CASE(UserDeleteMTest, CognitoMemoryDbTest) {

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
        const Entity::Cognito::User createdUser = _cognitoDatabase.CreateUser(user);

        // act
        _cognitoDatabase.DeleteUser(createdUser);
        const long count = _cognitoDatabase.CountUsers();

        // assert
        BOOST_CHECK_EQUAL(0, count);
    }

    BOOST_FIXTURE_TEST_CASE(ClientIdExistsMTest, CognitoMemoryDbTest) {

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

}// namespace AwsMock::Database::MemoryDb

#endif// AWMOCK_CORE_COGNITO_MEMORYDB_TEST_H