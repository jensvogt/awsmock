//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWMOCK_COGNITO_SERVERJAVATEST_H
#define AWMOCK_COGNITO_SERVERJAVATEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include "TestBase.h"
#include <awsmock/core/FileUtils.h>
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/CognitoDatabase.h>
#include <awsmock/service/cognito/CognitoServer.h>
#include <awsmock/service/gateway/GatewayServer.h>

#define REGION "eu-central-1"
#define TEST_USER_POOL "test-user-pool"
#define TEST_USER "test-user"
#define TEST_GROUP "test-group"
#define TEST_CLIENT "test-client"
#define TEST_DOMAIN "test-domain"
#define TEST_PORT 10100

namespace Awsmock::Service {

    /**
     * @brief Tests the aws-sdk-java interface to the AwsMock system.
     *
     * @par
     * The awsmock-test docker image will be started. The SNS server as well as the SQS server are started. This is needed as the SNS topic subscribe command needs an existing SQS queue.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class CognitoServerJavaTest : public testing::Test, public TestBase {

      protected:

        void SetUp() override {

            // General configuration
            StartGateway(TEST_PORT);
            _region = GetRegion();
            _endpoint = GetEndpoint();

            // Base URL
            _baseUrl = "/api/cognito/";
        }

        void TearDown() override {
            _database.DeleteAllUsers();
            _database.DeleteAllGroups();
            _database.DeleteAllUserPools();
        }

        std::string _endpoint, _baseUrl, _region;
        Database::CognitoDatabase _database = Database::CognitoDatabase();
    };

    TEST_F(CognitoServerJavaTest, UserPoolCreateTest) {

        // arrange

        // act
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        const Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();

        // assert
        EXPECT_TRUE(result.statusCode == http::status::ok);
        EXPECT_EQ(1, userPoolList.size());
    }

    TEST_F(CognitoServerJavaTest, UserPoolListTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        const Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());

        // act
        Core::HttpSocketResponse listResult = SendGetCommand(_baseUrl + "listUserPools?maxResults=10", {}, TEST_PORT);

        // assert
        EXPECT_TRUE(result.statusCode == http::status::ok);
    }

    TEST_F(CognitoServerJavaTest, UserPoolDescribeTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        const Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        const std::string userPoolId = userPoolList.front().userPoolId;

        // act
        Core::HttpSocketResponse listResult = SendGetCommand(_baseUrl + "describeUserPools?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId), {}, TEST_PORT);

        // assert
        EXPECT_TRUE(result.statusCode == http::status::ok);
    }

    TEST_F(CognitoServerJavaTest, UserPoolCreateClientTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        const std::string userPoolId = userPoolList.front().userPoolId;

        // act
        const Core::HttpSocketResponse createClientResult = SendGetCommand(_baseUrl + "createUserPoolClient?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&clientName=" + Core::StringUtils::UrlEncode(TEST_CLIENT), {}, TEST_PORT);
        EXPECT_TRUE(createClientResult.statusCode == http::status::ok);
        userPoolList = _database.ListUserPools();

        // assert
        EXPECT_EQ(1, userPoolList.size());
        EXPECT_EQ(1, userPoolList.front().userPoolClients.size());
    }

    TEST_F(CognitoServerJavaTest, UserPoolCreateDomainTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        const std::string userPoolId = userPoolList.front().userPoolId;

        // act
        const Core::HttpSocketResponse createDomainResult = SendGetCommand(_baseUrl + "createUserPoolDomain?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&domainName=" + Core::StringUtils::UrlEncode(TEST_DOMAIN), {}, TEST_PORT);
        EXPECT_TRUE(createDomainResult.statusCode == http::status::ok);
        userPoolList = _database.ListUserPools();

        // assert
        EXPECT_EQ(1, userPoolList.size());
        EXPECT_TRUE(userPoolList.front().domain.domain == TEST_DOMAIN);
    }

    TEST_F(CognitoServerJavaTest, UserPoolDeleteTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        const Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        const std::string userPoolId = userPoolList.front().userPoolId;

        // act
        const Core::HttpSocketResponse deleteResult = SendDeleteCommand(_baseUrl + "deleteUserPool?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId), {}, TEST_PORT);
        const long count = _database.CountUserPools();

        // assert
        EXPECT_TRUE(deleteResult.statusCode == http::status::ok);
        EXPECT_EQ(0, count);
    }

    TEST_F(CognitoServerJavaTest, UserCreateTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        const std::string userPoolId = userPoolList.front().userPoolId;

        // act
        const Core::HttpSocketResponse userCreateResult = SendPostCommand(_baseUrl + "createUser?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        const Database::Entity::Cognito::UserList userList = _database.ListUsers();

        // assert
        EXPECT_TRUE(userCreateResult.statusCode == http::status::ok);
        EXPECT_EQ(1, userList.size());
    }

    TEST_F(CognitoServerJavaTest, UserEnableTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        std::string userPoolId = userPoolList.front().userPoolId;
        Core::HttpSocketResponse userCreateResult = SendPostCommand(_baseUrl + "createUser?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        EXPECT_TRUE(userCreateResult.statusCode == http::status::ok);

        // act
        Core::HttpSocketResponse userEnableResult = SendPostCommand(_baseUrl + "enableUser?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        Database::Entity::Cognito::UserList userList = _database.ListUsers();

        // assert
        EXPECT_TRUE(userEnableResult.statusCode == http::status::ok);
        EXPECT_TRUE(userList.front().enabled);
    }

    TEST_F(CognitoServerJavaTest, UserDisableTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        std::string userPoolId = userPoolList.front().userPoolId;
        Core::HttpSocketResponse userCreateResult = SendPostCommand(_baseUrl + "createUser?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        EXPECT_TRUE(userCreateResult.statusCode == http::status::ok);

        // act
        Core::HttpSocketResponse userDisableResult = SendPostCommand(_baseUrl + "disableUser?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        Database::Entity::Cognito::UserList userList = _database.ListUsers();

        // assert
        EXPECT_TRUE(userDisableResult.statusCode == http::status::ok);
        EXPECT_FALSE(userList.front().enabled);
    }

    /*TEST_F(CognitoServerJavaTest, UserSignupTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL),{}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        std::string userPoolId = userPoolList.front().userPoolId;
        Core::HttpSocketResponse createClientResult = SendGetCommand(_baseUrl + "createUserPoolClient?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&clientName=" + Core::StringUtils::UrlEncode(TEST_CLIENT),{}, TEST_PORT);
        EXPECT_TRUE(createClientResult.statusCode == http::status::ok);
        userPoolList = _database.ListUserPools();
        std::string clientId = userPoolList.front().userPoolClients.front().clientId;
        std::string password = Core::StringUtils::GenerateRandomPassword(8);

        // act
        Core::HttpSocketResponse userSignupResult = SendPostCommand(_baseUrl + "signupUser?userName=" + Core::StringUtils::UrlEncode(TEST_USER) + "&clientId=" + Core::StringUtils::UrlEncode(clientId) + "&password=" + Core::StringUtils::UrlEncode(password),{}, TEST_PORT);
        Database::Entity::Cognito::UserList userList = _database.ListUsers();

        // assert
        EXPECT_TRUE(userSignupResult.statusCode == http::status::ok);
        EXPECT_TRUE(userList.front().enabled);
        EXPECT_TRUE(userList.front().userStatus == Database::Entity::Cognito::UserStatus::UNCONFIRMED);
    }*/
    /*
    TEST_F(CognitoServerJavaTest, UserConfirmTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL),{}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        std::string userPoolId = userPoolList.front().userPoolId;
        Core::HttpSocketResponse createClientResult = SendGetCommand(_baseUrl + "createUserPoolClient?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&clientName=" + Core::StringUtils::UrlEncode(TEST_CLIENT),{}, TEST_PORT);
        EXPECT_TRUE(createClientResult.statusCode == http::status::ok);
        userPoolList = _database.ListUserPools();
        std::string clientId = userPoolList.front().userPoolClients.front().clientId;
        std::string password = Core::StringUtils::GenerateRandomPassword(8);
        Core::HttpSocketResponse userSignupResult = SendPostCommand(_baseUrl + "signupUser?userName=" + Core::StringUtils::UrlEncode(TEST_USER) + "&clientId=" + Core::StringUtils::UrlEncode(clientId) + "&password=" + Core::StringUtils::UrlEncode(password),{}, TEST_PORT);

        // act
        Core::HttpSocketResponse confirmUserResult = SendPostCommand(_baseUrl + "adminConfirmSignUp?userPoolId=" + userPoolId + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER),{}, TEST_PORT);
        Database::Entity::Cognito::UserList userList = _database.ListUsers();

        // assert
        EXPECT_TRUE(confirmUserResult.statusCode == http::status::ok);
        EXPECT_TRUE(userList.front().enabled);
        EXPECT_TRUE(userList.front().userStatus == Database::Entity::Cognito::UserStatus::CONFIRMED);
    }
*/
    TEST_F(CognitoServerJavaTest, UserDeleteTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        std::string userPoolId = userPoolList.front().userPoolId;
        Core::HttpSocketResponse userCreateResult = SendPostCommand(_baseUrl + "createUser?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        Database::Entity::Cognito::UserList userList = _database.ListUsers();
        EXPECT_EQ(1, userList.size());

        // act
        Core::HttpSocketResponse deleteResult = SendDeleteCommand(_baseUrl + "deleteUser?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        long count = _database.CountUsers();

        // assert
        EXPECT_TRUE(deleteResult.statusCode == http::status::ok);
        EXPECT_EQ(0, count);
    }

    TEST_F(CognitoServerJavaTest, GroupCreateTest) {

        // arrange
        const Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        const Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        const std::string userPoolId = userPoolList.front().userPoolId;

        // act
        const Core::HttpSocketResponse groupCreateResult = SendPostCommand(_baseUrl + "createUserGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP), {}, TEST_PORT);
        const Database::Entity::Cognito::GroupList groupList = _database.ListGroups();

        // assert
        EXPECT_TRUE(groupCreateResult.statusCode == http::status::ok);
        EXPECT_EQ(1, groupList.size());
        EXPECT_TRUE(groupList.front().groupName == TEST_GROUP);
    }

    TEST_F(CognitoServerJavaTest, GroupListTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        std::string userPoolId = userPoolList.front().userPoolId;
        Core::HttpSocketResponse groupCreateResult = SendPostCommand(_baseUrl + "createUserGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP), {}, TEST_PORT);
        Database::Entity::Cognito::GroupList groupList = _database.ListGroups();
        EXPECT_EQ(1, groupList.size());

        // act
        Core::HttpSocketResponse listResult = SendGetCommand(_baseUrl + "listUserPools?maxResults=10", {}, TEST_PORT);

        // assert
        EXPECT_TRUE(result.statusCode == http::status::ok);
    }

    TEST_F(CognitoServerJavaTest, AddUserToGroupTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        std::string userPoolId = userPoolList.front().userPoolId;
        Core::HttpSocketResponse userCreateResult = SendPostCommand(_baseUrl + "createUser?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        Database::Entity::Cognito::UserList userList = _database.ListUsers();
        EXPECT_EQ(1, userList.size());
        Core::HttpSocketResponse userGroupCreateResult = SendPostCommand(_baseUrl + "createUserGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP), {}, TEST_PORT);
        EXPECT_TRUE(userGroupCreateResult.statusCode == http::status::ok);
        Database::Entity::Cognito::GroupList groupList = _database.ListGroups();
        EXPECT_EQ(1, groupList.size());

        // act
        Core::HttpSocketResponse addUserToCreateResult = SendPostCommand(_baseUrl + "addUserToGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        userList = _database.ListUsers();

        // assert
        EXPECT_TRUE(addUserToCreateResult.statusCode == http::status::ok);
        EXPECT_EQ(1, userList.size());
        EXPECT_EQ(1, userList.front().groups.size());
        EXPECT_TRUE(userList.front().groups.front().groupName == TEST_GROUP);
    }

    TEST_F(CognitoServerJavaTest, ListUsersInGroupTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        std::string userPoolId = userPoolList.front().userPoolId;
        Core::HttpSocketResponse userCreateResult = SendPostCommand(_baseUrl + "createUser?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        Database::Entity::Cognito::UserList userList = _database.ListUsers();
        EXPECT_EQ(1, userList.size());
        Core::HttpSocketResponse userGroupCreateResult = SendPostCommand(_baseUrl + "createUserGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP), {}, TEST_PORT);
        EXPECT_TRUE(userGroupCreateResult.statusCode == http::status::ok);
        Database::Entity::Cognito::GroupList groupList = _database.ListGroups();
        EXPECT_EQ(1, groupList.size());
        Core::HttpSocketResponse addUserToCreateResult = SendPostCommand(_baseUrl + "addUserToGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        EXPECT_TRUE(addUserToCreateResult.statusCode == http::status::ok);

        // act
        Core::HttpSocketResponse listUsersInGroupResult = SendGetCommand(_baseUrl + "listUsersInGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP), {}, TEST_PORT);
        int count = std::stoi(listUsersInGroupResult.body);

        // assert
        EXPECT_TRUE(listUsersInGroupResult.statusCode == http::status::ok);
        EXPECT_EQ(1, count);
    }

    TEST_F(CognitoServerJavaTest, RemoveUserFromGroupTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        std::string userPoolId = userPoolList.front().userPoolId;
        Core::HttpSocketResponse userCreateResult = SendPostCommand(_baseUrl + "createUser?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        Database::Entity::Cognito::UserList userList = _database.ListUsers();
        EXPECT_EQ(1, userList.size());
        Core::HttpSocketResponse userGroupCreateResult = SendPostCommand(_baseUrl + "createUserGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP), {}, TEST_PORT);
        EXPECT_TRUE(userGroupCreateResult.statusCode == http::status::ok);
        Database::Entity::Cognito::GroupList groupList = _database.ListGroups();
        EXPECT_EQ(1, groupList.size());
        Core::HttpSocketResponse addUserToCreateResult = SendPostCommand(_baseUrl + "addUserToGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        EXPECT_TRUE(addUserToCreateResult.statusCode == http::status::ok);
        userList = _database.ListUsers();
        EXPECT_EQ(1, userList.size());

        // act
        Core::HttpSocketResponse removeUserFromCreateResult = SendPostCommand(_baseUrl + "removeUserFromGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP) + "&userName=" + Core::StringUtils::UrlEncode(TEST_USER), {}, TEST_PORT);
        userList = _database.ListUsers();

        // assert
        EXPECT_TRUE(removeUserFromCreateResult.statusCode == http::status::ok);
        EXPECT_EQ(1, userList.size());
        EXPECT_EQ(0, userList.front().groups.size());
    }

    TEST_F(CognitoServerJavaTest, GroupDeleteTest) {

        // arrange
        Core::HttpSocketResponse result = SendPostCommand(_baseUrl + "createUserPool?name=" + Core::StringUtils::UrlEncode(TEST_USER_POOL), {}, TEST_PORT);
        EXPECT_TRUE(result.statusCode == http::status::ok);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        EXPECT_EQ(1, userPoolList.size());
        std::string userPoolId = userPoolList.front().userPoolId;
        Core::HttpSocketResponse userCreateResult = SendPostCommand(_baseUrl + "createUserGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP), {}, TEST_PORT);
        Database::Entity::Cognito::GroupList userGroupList = _database.ListGroups();
        EXPECT_EQ(1, userGroupList.size());

        // act
        Core::HttpSocketResponse deleteDeleteResult = SendDeleteCommand(_baseUrl + "deleteUserGroup?userPoolId=" + Core::StringUtils::UrlEncode(userPoolId) + "&groupName=" + Core::StringUtils::UrlEncode(TEST_GROUP), {}, TEST_PORT);
        userGroupList = _database.ListGroups();

        // assert
        EXPECT_TRUE(userCreateResult.statusCode == http::status::ok);
        EXPECT_EQ(0, userGroupList.size());
    }

}// namespace Awsmock::Service

#endif// AWMOCK_COGNITO_SERVERJAVATEST_H