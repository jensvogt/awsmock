//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_COGNITO_SERVICE_H
#define AWSMOCK_SERVICE_COGNITO_SERVICE_H

// C++ standard includes
#include <string>

// OpenSSL includes
#include <openssl/bn.h>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/core/JwtUtils.h>
#include <awsmock/core/SrpUtils.h>
#include <awsmock/core/exception/BadRequestException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/dto/cognito/AdminAddUserToGroupRequest.h>
#include <awsmock/dto/cognito/AdminConfirmUserRequest.h>
#include <awsmock/dto/cognito/AdminCreateUserRequest.h>
#include <awsmock/dto/cognito/AdminCreateUserResponse.h>
#include <awsmock/dto/cognito/AdminDeleteUserRequest.h>
#include <awsmock/dto/cognito/AdminDisableUserRequest.h>
#include <awsmock/dto/cognito/AdminEnableUserRequest.h>
#include <awsmock/dto/cognito/AdminGetUserRequest.h>
#include <awsmock/dto/cognito/AdminGetUserResponse.h>
#include <awsmock/dto/cognito/AdminRemoveUserFromGroupRequest.h>
#include <awsmock/dto/cognito/CreateGroupRequest.h>
#include <awsmock/dto/cognito/CreateGroupResponse.h>
#include <awsmock/dto/cognito/CreateUserPoolClientRequest.h>
#include <awsmock/dto/cognito/CreateUserPoolClientResponse.h>
#include <awsmock/dto/cognito/CreateUserPoolDomainRequest.h>
#include <awsmock/dto/cognito/CreateUserPoolDomainResponse.h>
#include <awsmock/dto/cognito/CreateUserPoolRequest.h>
#include <awsmock/dto/cognito/CreateUserPoolResponse.h>
#include <awsmock/dto/cognito/DeleteGroupRequest.h>
#include <awsmock/dto/cognito/DeleteUserPoolClientRequest.h>
#include <awsmock/dto/cognito/DeleteUserPoolRequest.h>
#include <awsmock/dto/cognito/DescribeUserPoolClientRequest.h>
#include <awsmock/dto/cognito/DescribeUserPoolClientResponse.h>
#include <awsmock/dto/cognito/DescribeUserPoolDomainRequest.h>
#include <awsmock/dto/cognito/DescribeUserPoolDomainResponse.h>
#include <awsmock/dto/cognito/DescribeUserPoolRequest.h>
#include <awsmock/dto/cognito/DescribeUserPoolResponse.h>
#include <awsmock/dto/cognito/GlobalSignOutRequest.h>
#include <awsmock/dto/cognito/InitiateAuthRequest.h>
#include <awsmock/dto/cognito/InitiateAuthResponse.h>
#include <awsmock/dto/cognito/ListGroupsRequest.h>
#include <awsmock/dto/cognito/ListGroupsResponse.h>
#include <awsmock/dto/cognito/ListUserPoolClientsRequest.h>
#include <awsmock/dto/cognito/ListUserPoolClientsResponse.h>
#include <awsmock/dto/cognito/ListUserPoolRequest.h>
#include <awsmock/dto/cognito/ListUserPoolResponse.h>
#include <awsmock/dto/cognito/ListUsersInGroupRequest.h>
#include <awsmock/dto/cognito/ListUsersInGroupResponse.h>
#include <awsmock/dto/cognito/ListUsersRequest.h>
#include <awsmock/dto/cognito/ListUsersResponse.h>
#include <awsmock/dto/cognito/RespondToAuthChallengeRequest.h>
#include <awsmock/dto/cognito/RespondToAuthChallengeResponse.h>
#include <awsmock/dto/cognito/SignUpRequest.h>
#include <awsmock/dto/cognito/SignUpResponse.h>
#include <awsmock/dto/cognito/UpdateUserPoolClientRequest.h>
#include <awsmock/dto/cognito/UpdateUserPoolDomainRequest.h>
#include <awsmock/dto/cognito/UpdateUserPoolDomainResponse.h>
#include <awsmock/dto/cognito/UpdateUserPoolRequest.h>
#include <awsmock/dto/cognito/internal/ListUserCountersRequest.h>
#include <awsmock/dto/cognito/internal/ListUserCountersResponse.h>
#include <awsmock/dto/cognito/internal/ListUserPoolCountersRequest.h>
#include <awsmock/dto/cognito/internal/ListUserPoolCountersResponse.h>
#include <awsmock/dto/cognito/mapper/Mapper.h>
#include <awsmock/dto/cognito/model/ChallengeName.h>
#include <awsmock/repository/CognitoDatabase.h>
#include <awsmock/service/secretsmanager/SecretsManagerService.h>

namespace AwsMock::Service {

    using std::chrono::system_clock;

    /**
     * @brief Cognito service
     *
     * Handles all Cognito requests finally.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class CognitoService {

      public:

        /**
         * @brief Constructor
         */
        explicit CognitoService();

        /**
         * @brief Create a new cognito user pool
         *
         * @param request create a user pool request
         * @return CreateUserPoolRequest DTO
         */
        Dto::Cognito::CreateUserPoolResponse CreateUserPool(const Dto::Cognito::CreateUserPoolRequest &request) const;

        /**
         * @brief Updates an existing cognito user pool
         *
         * @param request update user pool request
         */
        void UpdateUserPool(const Dto::Cognito::UpdateUserPoolRequest &request) const;

        /**
         * @brief List cognito user pools
         *
         * @param request list user pool request
         * @return ListUserPoolResponse DTO
         */
        Dto::Cognito::ListUserPoolResponse ListUserPools(const Dto::Cognito::ListUserPoolRequest &request) const;

        /**
         * @brief List cognito user pool counters
         *
         * @param request list user pool counters request
         * @return ListUserPoolCountersResponse DTO
         * @see ListUserPoolCountersResponse
         */
        Dto::Cognito::ListUserPoolCountersResponse ListUserPoolCounters(const Dto::Cognito::ListUserPoolCountersRequest &request) const;

        /**
         * @brief Describes a user pool
         *
         * @param request describe user pool request
         * @return DescribeUserPoolResponse DTO
         */
        Dto::Cognito::DescribeUserPoolResponse DescribeUserPool(const Dto::Cognito::DescribeUserPoolRequest &request) const;

        /**
         * @brief Delete a cognito user pool
         *
         * @param request delete user pool request
         */
        void DeleteUserPool(const Dto::Cognito::DeleteUserPoolRequest &request) const;

        /**
         * @brief Create a new cognito user pool domain
         *
         * @param request create user pool domain request
         * @return CreateUserPoolDomainResponse DTO
         */
        Dto::Cognito::CreateUserPoolDomainResponse CreateUserPoolDomain(const Dto::Cognito::CreateUserPoolDomainRequest &request) const;

        /**
         * @brief Updates an existing cognito user pool domain
         *
         * @param request update user pool domain request
         * @return UpdateUserPoolDomainResponse
         * @see UpdateUserPoolDomainResponse
         */
        Dto::Cognito::UpdateUserPoolDomainResponse UpdateUserPoolDomain(const Dto::Cognito::UpdateUserPoolDomainRequest &request) const;

        /**
         * @brief Describes a user pool domain
         *
         * @param request describe user pool domain request
         * @return DescribeUserPoolDomainResponse DTO
         * @see DescribeUserPoolDomainResponse DTO
         */
        static Dto::Cognito::DescribeUserPoolDomainResponse DescribeUserPoolDomain(const Dto::Cognito::DescribeUserPoolDomainRequest &request);

        /**
         * @brief Create a new cognito user pool client
         *
         * @param request create user pool client request
         * @return CreateUserPoolClientResponse DTO
         */
        Dto::Cognito::CreateUserPoolClientResponse CreateUserPoolClient(const Dto::Cognito::CreateUserPoolClientRequest &request) const;

        /**
         * @brief List cognito user pool clients
         *
         * @param request list user pool clients request
         * @return ListUserPoolClientsResponse DTO
         */
        Dto::Cognito::ListUserPoolClientsResponse ListUserPoolClients(const Dto::Cognito::ListUserPoolClientsRequest &request) const;

        /**
         * @brief Describes a user pool client
         *
         * @param request describe user pool client request
         * @return DescribeUserPoolResponse DTO
         */
        Dto::Cognito::DescribeUserPoolClientResponse DescribeUserPoolClient(const Dto::Cognito::DescribeUserPoolClientRequest &request) const;

        /**
         * @brief Updates an existing cognito user pool client
         *
         * @param request update user pool client request
         */
        void UpdateUserPoolClient(const Dto::Cognito::UpdateUserPoolClientRequest &request) const;

        /**
         * @brief Deletes a cognito user pool client
         *
         * @param request delete user pool client request
         */
        void DeleteUserPoolClient(const Dto::Cognito::DeleteUserPoolClientRequest &request) const;

        /**
         * @brief List cognito users
         *
         * @param request list user request
         * @return ListUsersResponse DTO
         */
        Dto::Cognito::ListUsersResponse ListUsers(const Dto::Cognito::ListUsersRequest &request) const;

        /**
         * @brief List cognito user counters
         *
         * @param request list user counters request
         * @return ListUserCountersResponse DTO
         * @see ListUserCountersResponse
         */
        Dto::Cognito::ListUserCountersResponse ListUserCounters(const Dto::Cognito::ListUserCountersRequest &request) const;

        /**
         * @brief Create a new cognito user pool
         *
         * @param request create a user pool request
         * @return CreateUserPoolResponse DTO
         */
        Dto::Cognito::CreateGroupResponse CreateGroup(const Dto::Cognito::CreateGroupRequest &request) const;

        /**
         * @brief Lists all available groups
         *
         * @param request list groups request
         * @return ListGroupsResponse DTO
         */
        Dto::Cognito::ListGroupsResponse ListGroups(const Dto::Cognito::ListGroupsRequest &request) const;

        /**
         * @brief Delete a cognito group
         *
         * @param request delete group request
         */
        void DeleteGroup(const Dto::Cognito::DeleteGroupRequest &request) const;

        /**
         * @brief Sign up a user
         *
         * @param request sign up request
         * @return SignUpResponse DTO
         */
        Dto::Cognito::SignUpResponse SignUp(const Dto::Cognito::SignUpRequest &request) const;

        /**
         * @brief Confirm a user
         *
         * @param request confirm user request
         */
        void ConfirmUser(const Dto::Cognito::AdminConfirmUserRequest &request) const;

        /**
         * @brief Initiate authentication
         *
         * @par
         * AWS Cognito challenges page: https://docs.aws.amazon.com/cognito/latest/developerguide/amazon-cognito-user-pools-authentication-flow-methods.html#amazon-cognito-user-pools-authentication-flow-methods-password
         *
         * @par
         * For USER_SRP see: https://en.wikipedia.org/wiki/Secure_Remote_Password_protocol
         *
         * @param request confirm user request
         * @return InitiateAuthResponse DTO
         * @see InitiateAuthRequest
         * @see InitiateAuthResponse
         */
        Dto::Cognito::InitiateAuthResponse InitiateAuth(Dto::Cognito::InitiateAuthRequest &request) const;

        /**
         * @brief Respond to an auth challenge
         *
         * @par
         * Some API operations in a user pool generate a challenge, like a prompt for an MFA code, for device authentication that bypasses MFA, or for a custom authentication
         * challenge. A RespondToAuthChallenge API request provides the answer to that challenge, like a code or a secure remote password (SRP). The parameters of a response
         * to an authentication challenge vary with the type of challenge.
         *
         * @param request respond to auth challenge request
         * @return RespondToAuthChallengeResponse DTO
         * @see RespondToAuthChallengeRequest
         * @see RespondToAuthChallengeResponse
         */
        Dto::Cognito::RespondToAuthChallengeResponse RespondToAuthChallenge(Dto::Cognito::RespondToAuthChallengeRequest &request) const;

        /**
         * @brief Global sign-out request
         *
         * @param request sign out request
         * @see GlobalSignOutRequest
         */
        static void GlobalSignOut(const Dto::Cognito::GlobalSignOutRequest &request);

        /**
         * @brief Create a new cognito user
         *
         * @param request create user request
         * @return AdminCreateUserResponse DTO
         */
        Dto::Cognito::AdminCreateUserResponse AdminCreateUser(const Dto::Cognito::AdminCreateUserRequest &request) const;

        /**
         * @brief Get a local cognito user
         *
         * @param request get user request
         * @return AdminGetUserResponse DTO
         */
        Dto::Cognito::AdminGetUserResponse AdminGetUser(const Dto::Cognito::AdminGetUserRequest &request) const;

        /**
         * @brief Adds a user to a group
         *
         * @param request add user to group request
         */
        void AdminAddUserToGroup(const Dto::Cognito::AdminAddUserToGroupRequest &request) const;

        /**
         * @brief Removes a user from a group
         *
         * @param request remove user from group request
         */
        void AdminRemoveUserFromGroup(const Dto::Cognito::AdminRemoveUserFromGroupRequest &request) const;

        /**
         * @brief Lists the users in a group
         *
         * @param request list user in group request
         */
        Dto::Cognito::ListUsersInGroupResponse ListUsersInGroup(const Dto::Cognito::ListUsersInGroupRequest &request) const;

        /**
         * @brief Enable a user
         *
         * @param request enable user request
         */
        void AdminEnableUser(const Dto::Cognito::AdminEnableUserRequest &request) const;

        /**
         * @brief Disable a user
         *
         * @param request disable user request
         */
        void AdminDisableUser(const Dto::Cognito::AdminDisableUserRequest &request) const;

        /**
         * @brief Deletes a cognito user
         *
         * @param request delete user request
         */
        void AdminDeleteUser(const Dto::Cognito::AdminDeleteUserRequest &request) const;

      private:

        /**
         * Database connection
         */
        Database::CognitoDatabase &_database;

        /**
         * AWS account userPoolId
         */
        std::string _accountId;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_COGNITO_SERVICE_H
