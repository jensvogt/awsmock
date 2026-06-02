//
// Created by vogje01 on 30/05/2023.
//

// Boost includes
#include <boost/locale.hpp>
#include <boost/test/unit_test.hpp>

// AwsMock includes
#include <../../db/include/awsmock/repository/cognito/CognitoMongoRepository.h>
#include <awsmock/service/cognito/CognitoService.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

#define TEST_REGION "eu-central-1"
#define TEST_POOL_NAME "test-user-pool"
#define TEST_CLIENT_NAME "test-client"
#define TEST_USER_NAME "test-user"
#define TEST_PASSWORD "Test1234!"
#define TEST_GROUP_NAME "test-group"

namespace Awsmock::Database {

    Dto::Cognito::CreateUserPoolResponse CreateDefaultUserPool(const Service::CognitoService &cognitoService) {
        Dto::Cognito::CreateUserPoolRequest request;
        request.region = TEST_REGION;
        request.name = TEST_POOL_NAME;
        return cognitoService.CreateUserPool(request);
    }

    Dto::Cognito::AdminCreateUserResponse CreateDefaultUser(const Service::CognitoService &cognitoService, const std::string &userPoolId) {
        Dto::Cognito::AdminCreateUserRequest request;
        request.region = TEST_REGION;
        request.userPoolId = userPoolId;
        request.userName = TEST_USER_NAME;
        request.temporaryPassword = TEST_PASSWORD;
        return cognitoService.AdminCreateUser(request);
    }

    Dto::Cognito::CreateGroupResponse CreateDefaultGroup(const Service::CognitoService &cognitoService, const std::string &userPoolId) {
        Dto::Cognito::CreateGroupRequest request;
        request.region = TEST_REGION;
        request.userPoolId = userPoolId;
        request.groupName = TEST_GROUP_NAME;
        return cognitoService.CreateGroup(request);
    }

    struct CognitoServiceFixture {
        CognitoServiceFixture() = default;
        ~CognitoServiceFixture() {
            try {
                CognitoMongoRepository::instance().DeleteAllGroups();
                log_debug << "Cognito groups deleted";
                CognitoMongoRepository::instance().DeleteAllUsers();
                log_debug << "Cognito users deleted";
                CognitoMongoRepository::instance().DeleteAllUserPools();
                log_debug << "Cognito user pools deleted";
            } catch (const std::exception &exc) {
                log_error << "Cognito fixture cleanup failed: " << exc.what();
            }
        }
    };

    BOOST_FIXTURE_TEST_SUITE(CognitoServiceTests, CognitoServiceFixture)

    BOOST_AUTO_TEST_CASE(UserPoolCreateTest) {

        // arrange
        const Service::CognitoService cognitoService;
        Dto::Cognito::CreateUserPoolRequest request;
        request.region = TEST_REGION;
        request.name = TEST_POOL_NAME;

        // act
        const Dto::Cognito::CreateUserPoolResponse response = cognitoService.CreateUserPool(request);

        // assert
        BOOST_CHECK_EQUAL(TEST_POOL_NAME, response.userPool.name);
        BOOST_CHECK_EQUAL(false, response.userPool.userPoolId.empty());
        BOOST_CHECK_EQUAL(false, response.userPool.arn.empty());
    }

    BOOST_AUTO_TEST_CASE(UserPoolListTest) {

        // arrange
        const Service::CognitoService cognitoService;
        Dto::Cognito::CreateUserPoolRequest createRequest1;
        createRequest1.region = TEST_REGION;
        createRequest1.name = "test-user-pool-1";
        Dto::Cognito::CreateUserPoolResponse createResponse1 = cognitoService.CreateUserPool(createRequest1);
        BOOST_CHECK_EQUAL(false, createResponse1.userPool.userPoolId.empty());

        Dto::Cognito::CreateUserPoolRequest createRequest2;
        createRequest2.region = TEST_REGION;
        createRequest2.name = "test-user-pool-2";
        Dto::Cognito::CreateUserPoolResponse createResponse2 = cognitoService.CreateUserPool(createRequest2);
        BOOST_CHECK_EQUAL(false, createResponse2.userPool.userPoolId.empty());

        Dto::Cognito::ListUserPoolRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.maxResults = 10;

        // act
        const Dto::Cognito::ListUserPoolResponse response = cognitoService.ListUserPools(listRequest);

        // assert
        BOOST_CHECK_EQUAL(2, response.total);
        BOOST_CHECK_EQUAL(2, response.userPools.size());
    }

    BOOST_AUTO_TEST_CASE(UserPoolDescribeTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse createResponse = CreateDefaultUserPool(cognitoService);
        BOOST_CHECK_EQUAL(false, createResponse.userPool.userPoolId.empty());

        Dto::Cognito::DescribeUserPoolRequest describeRequest;
        describeRequest.region = TEST_REGION;
        describeRequest.userPoolId = createResponse.userPool.userPoolId;

        // act
        const Dto::Cognito::DescribeUserPoolResponse response = cognitoService.DescribeUserPool(describeRequest);

        // assert
        BOOST_CHECK_EQUAL(TEST_POOL_NAME, response.userPool.name);
        BOOST_CHECK_EQUAL(createResponse.userPool.userPoolId, response.userPool.userPoolId);
    }

    BOOST_AUTO_TEST_CASE(UserPoolDeleteTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse createResponse = CreateDefaultUserPool(cognitoService);
        BOOST_CHECK_EQUAL(false, createResponse.userPool.userPoolId.empty());

        Dto::Cognito::DeleteUserPoolRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.userPoolId = createResponse.userPool.userPoolId;

        // act
        BOOST_CHECK_NO_THROW(cognitoService.DeleteUserPool(deleteRequest));

        // assert
        Dto::Cognito::ListUserPoolRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.maxResults = 10;
        const Dto::Cognito::ListUserPoolResponse listResponse = cognitoService.ListUserPools(listRequest);
        BOOST_CHECK_EQUAL(0, listResponse.total);
    }

    BOOST_AUTO_TEST_CASE(UserPoolClientCreateTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse poolResponse = CreateDefaultUserPool(cognitoService);

        Dto::Cognito::CreateUserPoolClientRequest request;
        request.region = TEST_REGION;
        request.userPoolId = poolResponse.userPool.userPoolId;
        request.clientName = TEST_CLIENT_NAME;
        request.generateSecret = true;

        // act
        const Dto::Cognito::CreateUserPoolClientResponse response = cognitoService.CreateUserPoolClient(request);

        // assert
        BOOST_CHECK_EQUAL(TEST_CLIENT_NAME, response.userPoolClient.clientName);
        BOOST_CHECK_EQUAL(false, response.userPoolClient.clientId.empty());
        BOOST_CHECK_EQUAL(false, response.userPoolClient.clientSecret.empty());
    }

    BOOST_AUTO_TEST_CASE(UserPoolClientListTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse poolResponse = CreateDefaultUserPool(cognitoService);

        Dto::Cognito::CreateUserPoolClientRequest clientRequest1;
        clientRequest1.region = TEST_REGION;
        clientRequest1.userPoolId = poolResponse.userPool.userPoolId;
        clientRequest1.clientName = "test-client-1";
        Dto::Cognito::CreateUserPoolClientResponse createResponse1 = cognitoService.CreateUserPoolClient(clientRequest1);
        BOOST_CHECK_EQUAL(false, createResponse1.userPoolClient.clientId.empty());

        Dto::Cognito::CreateUserPoolClientRequest clientRequest2;
        clientRequest2.region = TEST_REGION;
        clientRequest2.userPoolId = poolResponse.userPool.userPoolId;
        clientRequest2.clientName = "test-client-2";
        Dto::Cognito::CreateUserPoolClientResponse createResponse2 = cognitoService.CreateUserPoolClient(clientRequest2);
        BOOST_CHECK_EQUAL(false, createResponse2.userPoolClient.clientId.empty());

        Dto::Cognito::ListUserPoolClientsRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.userPoolId = poolResponse.userPool.userPoolId;

        // act
        const Dto::Cognito::ListUserPoolClientsResponse response = cognitoService.ListUserPoolClients(listRequest);

        // assert
        BOOST_CHECK_EQUAL(false, response.userPoolsClients.empty());
        BOOST_CHECK_EQUAL(2, response.userPoolsClients.size());
    }

    BOOST_AUTO_TEST_CASE(UserPoolClientDeleteTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse poolResponse = CreateDefaultUserPool(cognitoService);

        Dto::Cognito::CreateUserPoolClientRequest clientRequest;
        clientRequest.region = TEST_REGION;
        clientRequest.userPoolId = poolResponse.userPool.userPoolId;
        clientRequest.clientName = TEST_CLIENT_NAME;
        const Dto::Cognito::CreateUserPoolClientResponse clientResponse = cognitoService.CreateUserPoolClient(clientRequest);
        BOOST_CHECK_EQUAL(false, clientResponse.userPoolClient.clientId.empty());

        Dto::Cognito::DeleteUserPoolClientRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.userPoolId = poolResponse.userPool.userPoolId;
        deleteRequest.clientId = clientResponse.userPoolClient.clientId;

        // act
        BOOST_CHECK_NO_THROW(cognitoService.DeleteUserPoolClient(deleteRequest));

        // assert
        Dto::Cognito::ListUserPoolClientsRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.userPoolId = poolResponse.userPool.userPoolId;
        const Dto::Cognito::ListUserPoolClientsResponse listResponse = cognitoService.ListUserPoolClients(listRequest);
        BOOST_CHECK_EQUAL(true, listResponse.userPoolsClients.empty());
    }

    BOOST_AUTO_TEST_CASE(AdminCreateUserTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse poolResponse = CreateDefaultUserPool(cognitoService);

        Dto::Cognito::AdminCreateUserRequest request;
        request.region = TEST_REGION;
        request.userPoolId = poolResponse.userPool.userPoolId;
        request.userName = TEST_USER_NAME;
        request.temporaryPassword = TEST_PASSWORD;

        // act
        const Dto::Cognito::AdminCreateUserResponse response = cognitoService.AdminCreateUser(request);

        // assert
        BOOST_CHECK_EQUAL(TEST_USER_NAME, response.user.userName);
        BOOST_CHECK_EQUAL(false, response.user.userName.empty());
    }

    BOOST_AUTO_TEST_CASE(AdminGetUserTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse poolResponse = CreateDefaultUserPool(cognitoService);
        CreateDefaultUser(cognitoService, poolResponse.userPool.userPoolId);

        Dto::Cognito::AdminGetUserRequest getRequest;
        getRequest.region = TEST_REGION;
        getRequest.userPoolId = poolResponse.userPool.userPoolId;
        getRequest.userName = TEST_USER_NAME;

        // act
        const Dto::Cognito::AdminGetUserResponse response = cognitoService.AdminGetUser(getRequest);

        // assert
        BOOST_CHECK_EQUAL(TEST_USER_NAME, response.userName);
        BOOST_CHECK_EQUAL(poolResponse.userPool.userPoolId, response.userPoolId);
    }

    BOOST_AUTO_TEST_CASE(ListUsersTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse poolResponse = CreateDefaultUserPool(cognitoService);

        Dto::Cognito::AdminCreateUserRequest userRequest1;
        userRequest1.region = TEST_REGION;
        userRequest1.userPoolId = poolResponse.userPool.userPoolId;
        userRequest1.userName = "test-user-1";
        userRequest1.temporaryPassword = TEST_PASSWORD;
        const Dto::Cognito::AdminCreateUserResponse createResponse1 = cognitoService.AdminCreateUser(userRequest1);
        BOOST_CHECK_EQUAL(false, createResponse1.user.userName.empty());

        Dto::Cognito::AdminCreateUserRequest userRequest2;
        userRequest2.region = TEST_REGION;
        userRequest2.userPoolId = poolResponse.userPool.userPoolId;
        userRequest2.userName = "test-user-2";
        userRequest2.temporaryPassword = TEST_PASSWORD;
        const Dto::Cognito::AdminCreateUserResponse createResponse2 = cognitoService.AdminCreateUser(userRequest2);
        BOOST_CHECK_EQUAL(false, createResponse2.user.userName.empty());

        Dto::Cognito::ListUsersRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.userPoolId = poolResponse.userPool.userPoolId;

        // act
        const Dto::Cognito::ListUsersResponse response = cognitoService.ListUsers(listRequest);

        // assert
        BOOST_CHECK_EQUAL(false, response.users.empty());
        BOOST_CHECK_EQUAL(2, response.users.size());
    }

    BOOST_AUTO_TEST_CASE(AdminDeleteUserTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse poolResponse = CreateDefaultUserPool(cognitoService);
        CreateDefaultUser(cognitoService, poolResponse.userPool.userPoolId);

        Dto::Cognito::AdminDeleteUserRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.userPoolId = poolResponse.userPool.userPoolId;
        deleteRequest.userName = TEST_USER_NAME;

        // act
        BOOST_CHECK_NO_THROW(cognitoService.AdminDeleteUser(deleteRequest));

        // assert
        Dto::Cognito::ListUsersRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.userPoolId = poolResponse.userPool.userPoolId;
        const Dto::Cognito::ListUsersResponse listResponse = cognitoService.ListUsers(listRequest);
        BOOST_CHECK_EQUAL(true, listResponse.users.empty());
    }

    BOOST_AUTO_TEST_CASE(CreateGroupTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse poolResponse = CreateDefaultUserPool(cognitoService);

        Dto::Cognito::CreateGroupRequest request;
        request.region = TEST_REGION;
        request.userPoolId = poolResponse.userPool.userPoolId;
        request.groupName = TEST_GROUP_NAME;

        // act
        const Dto::Cognito::CreateGroupResponse response = cognitoService.CreateGroup(request);

        // assert
        BOOST_CHECK_EQUAL(TEST_GROUP_NAME, response.group.groupName);
        BOOST_CHECK_EQUAL(poolResponse.userPool.userPoolId, response.group.userPoolId);
    }

    BOOST_AUTO_TEST_CASE(ListGroupsTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse poolResponse = CreateDefaultUserPool(cognitoService);

        Dto::Cognito::CreateGroupRequest groupRequest1;
        groupRequest1.region = TEST_REGION;
        groupRequest1.userPoolId = poolResponse.userPool.userPoolId;
        groupRequest1.groupName = "test-group-1";
        const Dto::Cognito::CreateGroupResponse createResponse1 = cognitoService.CreateGroup(groupRequest1);
        BOOST_CHECK_EQUAL(false, createResponse1.group.groupName.empty());

        Dto::Cognito::CreateGroupRequest groupRequest2;
        groupRequest2.region = TEST_REGION;
        groupRequest2.userPoolId = poolResponse.userPool.userPoolId;
        groupRequest2.groupName = "test-group-2";
        const Dto::Cognito::CreateGroupResponse createResponse2 = cognitoService.CreateGroup(groupRequest2);
        BOOST_CHECK_EQUAL(false, createResponse2.group.groupName.empty());

        Dto::Cognito::ListGroupsRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.userPoolId = poolResponse.userPool.userPoolId;
        listRequest.limit = 10;

        // act
        const Dto::Cognito::ListGroupsResponse response = cognitoService.ListGroups(listRequest);

        // assert
        BOOST_CHECK_EQUAL(false, response.groups.empty());
        BOOST_CHECK_EQUAL(2, response.groups.size());
    }

    BOOST_AUTO_TEST_CASE(AdminAddUserToGroupTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse poolResponse = CreateDefaultUserPool(cognitoService);
        CreateDefaultUser(cognitoService, poolResponse.userPool.userPoolId);
        CreateDefaultGroup(cognitoService, poolResponse.userPool.userPoolId);

        Dto::Cognito::AdminAddUserToGroupRequest request;
        request.region = TEST_REGION;
        request.userPoolId = poolResponse.userPool.userPoolId;
        request.userName = TEST_USER_NAME;
        request.groupName = TEST_GROUP_NAME;

        // act
        BOOST_CHECK_NO_THROW(cognitoService.AdminAddUserToGroup(request));

        // assert
        Dto::Cognito::AdminGetUserRequest getRequest;
        getRequest.region = TEST_REGION;
        getRequest.userPoolId = poolResponse.userPool.userPoolId;
        getRequest.userName = TEST_USER_NAME;
        const Dto::Cognito::AdminGetUserResponse getResponse = cognitoService.AdminGetUser(getRequest);
        BOOST_CHECK_EQUAL(TEST_USER_NAME, getResponse.userName);
    }

    BOOST_AUTO_TEST_CASE(DeleteGroupTest) {

        // arrange
        const Service::CognitoService cognitoService;
        const Dto::Cognito::CreateUserPoolResponse poolResponse = CreateDefaultUserPool(cognitoService);
        CreateDefaultGroup(cognitoService, poolResponse.userPool.userPoolId);

        Dto::Cognito::DeleteGroupRequest deleteRequest;
        deleteRequest.region = TEST_REGION;
        deleteRequest.userPoolId = poolResponse.userPool.userPoolId;
        deleteRequest.groupName = TEST_GROUP_NAME;

        // act
        BOOST_CHECK_NO_THROW(cognitoService.DeleteGroup(deleteRequest));

        // assert
        Dto::Cognito::ListGroupsRequest listRequest;
        listRequest.region = TEST_REGION;
        listRequest.userPoolId = poolResponse.userPool.userPoolId;
        listRequest.limit = 10;
        const Dto::Cognito::ListGroupsResponse listResponse = cognitoService.ListGroups(listRequest);
        BOOST_CHECK_EQUAL(true, listResponse.groups.empty());
    }

    BOOST_AUTO_TEST_SUITE_END()

}// namespace Awsmock::Database
