//
// Created by vogje01 on 02/06/2023.
//

#include "TestBase.h"
#ifndef AWMOCK_COGNITO_SERVER_CLI_TEST_H
#define AWMOCK_COGNITO_SERVER_CLI_TEST_H

// GTest includes
#include <gtest/gtest.h>

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/repository/S3Database.h>
#include <awsmock/service/cognito/CognitoServer.h>
#include <awsmock/service/cognito/CognitoService.h>
#include <awsmock/service/gateway/GatewayServer.h>

#define REGION "eu-central-1"
#define OWNER "test-owner"
#define ACCOUNT_ID "000000000000"
#define PASSWORD std::string("test-password")
#define USER_NAME std::string("test-user")
#define GROUP_NAME std::string("test-group")
#define CLIENT_NAME std::string("test-client")
#define DOMAIN_NAME std::string("test-domain")
#define USER_POOL_NAME std::string("test-user-pool")

namespace AwsMock::Service {

    /**
     * @brief AwsMock cognito integration test.
     */
    class CognitoServerCliTest : public testing::Test, public TestBase {

      protected:

        void SetUp() override {

            // General configuration
            StartGateway();

            _region = GetRegion();
            _endpoint = GetEndpoint();
        }

        void TearDown() override {
            _database.DeleteAllUsers();
            _database.DeleteAllGroups();
            _database.DeleteAllUserPools();
        }

        boost::thread _thread;
        std::string _endpoint, _region;
        Core::Configuration &_configuration = Core::Configuration::instance();
        Database::CognitoDatabase &_database = Database::CognitoDatabase::instance();
        std::shared_ptr<GatewayServer> _gatewayServer;
    };

    TEST_F(CognitoServerCliTest, UserPoolCreateTest) {

        // arrange

        // act
        auto [status, output] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        const Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();

        // assert
        EXPECT_EQ(0, status);
        EXPECT_EQ(1, userPoolList.size());
    }

    TEST_F(CognitoServerCliTest, UserPoolListTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);

        // act
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp list-user-pools --max-results 10 --endpoint " + _endpoint);

        // assert
        EXPECT_EQ(0, status2);
        EXPECT_FALSE(output2.empty());
        EXPECT_TRUE(Core::StringUtils::Contains(output2, USER_POOL_NAME));
    }

    TEST_F(CognitoServerCliTest, UserPoolCreateClientTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        const Database::Entity::Cognito::UserPoolList createdPoolList = _database.ListUserPools();
        const std::string userPoolId = createdPoolList.front().userPoolId;

        // act
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool-client --user-pool-id " + userPoolId + " --client-name " + CLIENT_NAME + " --endpoint " + _endpoint);
        const Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();

        // assert
        EXPECT_EQ(0, status2);
        EXPECT_EQ(1, userPoolList.size());
        EXPECT_FALSE(userPoolList.front().userPoolClients.empty());
    }

    TEST_F(CognitoServerCliTest, UserPoolCreateDomainTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        const Database::Entity::Cognito::UserPoolList createdPoolList = _database.ListUserPools();
        const std::string userPoolId = createdPoolList.front().userPoolId;

        // act
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool-domain --user-pool-id " + userPoolId + " --domain " + DOMAIN_NAME + " --endpoint " + _endpoint);
        const Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();

        // assert
        EXPECT_EQ(0, status2);
        EXPECT_EQ(1, userPoolList.size());
        EXPECT_TRUE(userPoolList.front().domain.domain == DOMAIN_NAME);
    }

    TEST_F(CognitoServerCliTest, UserPoolDeleteTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        const Database::Entity::Cognito::UserPoolList createdPoolList = _database.ListUserPools();
        const std::string userPoolId = createdPoolList.front().userPoolId;

        // act
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp delete-user-pool --user-pool-id " + userPoolId + "  --endpoint " + _endpoint);
        const Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();

        // assert
        EXPECT_EQ(0, status2);
        EXPECT_EQ(0, userPoolList.size());
    }

    TEST_F(CognitoServerCliTest, UserCreateTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        const Database::Entity::Cognito::UserPoolList createdPoolList = _database.ListUserPools();
        const std::string userPoolId = createdPoolList.front().userPoolId;

        // act
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-create-user --user-pool-id " + userPoolId + " --username " + USER_NAME + " --endpoint " + _endpoint);
        const Database::Entity::Cognito::UserList userList = _database.ListUsers();

        // assert
        EXPECT_EQ(0, status2);
        EXPECT_EQ(1, userList.size());
    }

    TEST_F(CognitoServerCliTest, UserEnableTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        const Database::Entity::Cognito::UserPoolList createdPoolList = _database.ListUserPools();
        const std::string userPoolId = createdPoolList.front().userPoolId;
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-create-user --user-pool-id " + userPoolId + " --username " + USER_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status2);
        Database::Entity::Cognito::UserList userList = _database.ListUsers();
        EXPECT_EQ(1, userList.size());

        // act
        auto [status3, output3] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-enable-user --user-pool-id " + userPoolId + " --username " + USER_NAME + " --endpoint " + _endpoint);
        userList = _database.ListUsers();

        // assert
        EXPECT_EQ(0, status3);
        EXPECT_EQ(1, userList.size());
        EXPECT_TRUE(userList.front().enabled);
    }

    TEST_F(CognitoServerCliTest, UserDisableTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        const Database::Entity::Cognito::UserPoolList createdPoolList = _database.ListUserPools();
        const std::string userPoolId = createdPoolList.front().userPoolId;

        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-create-user --user-pool-id " + userPoolId + " --username " + USER_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status2);
        Database::Entity::Cognito::UserList userList = _database.ListUsers();
        EXPECT_EQ(1, userList.size());

        auto [status3, output3] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-enable-user --user-pool-id " + userPoolId + " --username " + USER_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status3);

        // act
        auto [status4, output4] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-disable-user --user-pool-id " + userPoolId + " --username " + USER_NAME + " --endpoint " + _endpoint);
        userList = _database.ListUsers();

        // assert
        EXPECT_EQ(0, status4);
        EXPECT_EQ(1, userList.size());
        EXPECT_FALSE(userList.front().enabled);
    }

    TEST_F(CognitoServerCliTest, UserDeleteTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        const Database::Entity::Cognito::UserPoolList createdPoolList = _database.ListUserPools();
        const std::string userPoolId = createdPoolList.front().userPoolId;
        Core::ExecResult createUserResult = Core::TestUtils::SendCliCommand("aws cognito-idp admin-create-user --user-pool-id " + userPoolId + " --username " + USER_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);

        // act
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-delete-user --user-pool-id " + userPoolId + " --username " + USER_NAME + " --endpoint " + _endpoint);
        const Database::Entity::Cognito::UserList userList = _database.ListUsers();

        // assert
        EXPECT_EQ(0, status2);
        EXPECT_EQ(0, userList.size());
    }

    TEST_F(CognitoServerCliTest, GroupCreateTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        const Database::Entity::Cognito::UserPoolList createdPoolList = _database.ListUserPools();
        const std::string userPoolId = createdPoolList.front().userPoolId;

        // act
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp create-group --user-pool-id " + userPoolId + " --group-name " + GROUP_NAME + " --endpoint " + _endpoint);
        const Database::Entity::Cognito::GroupList groupList = _database.ListGroups();

        // assert
        EXPECT_EQ(0, status2);
        EXPECT_EQ(1, groupList.size());
    }

    TEST_F(CognitoServerCliTest, GroupDeleteTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        const Database::Entity::Cognito::UserPoolList createdPoolList = _database.ListUserPools();
        const std::string userPoolId = createdPoolList.front().userPoolId;
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp create-group --user-pool-id " + userPoolId + " --group-name " + GROUP_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status2);

        // act
        auto [status3, output3] = Core::TestUtils::SendCliCommand("aws cognito-idp delete-group --user-pool-id " + userPoolId + " --group-name " + GROUP_NAME + " --endpoint " + _endpoint);
        const Database::Entity::Cognito::GroupList groupList = _database.ListGroups();

        // assert
        EXPECT_EQ(0, status3);
        EXPECT_EQ(0, groupList.size());
    }

    TEST_F(CognitoServerCliTest, UserAddToGroupTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        const Database::Entity::Cognito::UserPoolList createdPoolList = _database.ListUserPools();

        const std::string userPoolId = createdPoolList.front().userPoolId;
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp create-group --user-pool-id " + userPoolId + " --group-name " + GROUP_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status2);

        auto [status3, output3] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-create-user --user-pool-id " + userPoolId + " --username " + USER_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status3);

        // act
        auto [status4, output4] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-add-user-to-group --user-pool-id " + userPoolId + " --group-name " + GROUP_NAME + " --username " + USER_NAME + " --endpoint " + _endpoint);
        const Database::Entity::Cognito::UserList userList = _database.ListUsers();

        // assert
        EXPECT_EQ(0, status4);
        EXPECT_EQ(1, userList.size());
        EXPECT_EQ(1, userList.front().groups.size());
        EXPECT_TRUE(userList.front().groups.front().groupName == GROUP_NAME);
    }

    TEST_F(CognitoServerCliTest, UserRemoveFromGroupTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        Database::Entity::Cognito::UserPoolList createdPoolList = _database.ListUserPools();
        std::string userPoolId = createdPoolList.front().userPoolId;
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp create-group --user-pool-id " + userPoolId + " --group-name " + GROUP_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status2);
        auto [status3, output3] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-create-user --user-pool-id " + userPoolId + " --username " + USER_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status3);
        auto [status4, output4] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-add-user-to-group --user-pool-id " + userPoolId + " --group-name " + GROUP_NAME + " --username " + USER_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status4);

        // act
        auto [status5, output5] = Core::TestUtils::SendCliCommand("aws cognito-idp admin-remove-user-from-group --user-pool-id " + userPoolId + " --group-name " + GROUP_NAME + " --username " + USER_NAME + " --endpoint " + _endpoint);
        Database::Entity::Cognito::UserList userList = _database.ListUsers();

        // assert
        EXPECT_EQ(0, status5);
        EXPECT_EQ(1, userList.size());
        EXPECT_EQ(0, userList.front().groups.size());
    }

    TEST_F(CognitoServerCliTest, UserSignUpTest) {

        // arrange
        auto [status1, output1] = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool --pool-name " + USER_POOL_NAME + " --endpoint " + _endpoint);
        EXPECT_EQ(0, status1);
        Database::Entity::Cognito::UserPoolList userPoolList = _database.ListUserPools();
        const std::string userPoolId = userPoolList.front().userPoolId;
        Core::ExecResult createUserPoolClientResult = Core::TestUtils::SendCliCommand("aws cognito-idp create-user-pool-client --user-pool-id " + userPoolId + " --client-name " + CLIENT_NAME + " --endpoint " + _endpoint);
        userPoolList = _database.ListUserPools();
        const std::string clientId = userPoolList.front().userPoolClients.front().clientId;

        // act
        auto [status2, output2] = Core::TestUtils::SendCliCommand("aws cognito-idp sign-up --client-id " + clientId + " --username " + USER_NAME + " --password " + PASSWORD + " --endpoint " + _endpoint);
        const Database::Entity::Cognito::UserList userList = _database.ListUsers();

        // assert
        EXPECT_EQ(0, status2);
        EXPECT_EQ(1, userList.size());
        EXPECT_TRUE(userList.front().enabled);
        EXPECT_TRUE(userList.front().userStatus == Database::Entity::Cognito::UserStatus::UNCONFIRMED);
    }

}// namespace AwsMock::Service

#endif// AWMOCK_COGNITO_SERVER_CLI_TEST_H