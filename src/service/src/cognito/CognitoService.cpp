
//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/cognito/CognitoService.h>

namespace AwsMock::Service {

    CognitoService::CognitoService() : _database(Database::CognitoDatabase::instance()) {
        Core::Configuration &configuration = Core::Configuration::instance();
        _accountId = configuration.getString("aws.accountId", DEFAULT_ACCOUNT_ID);
    }

    Dto::Cognito::CreateUserPoolResponse CognitoService::CreateUserPool(const Dto::Cognito::CreateUserPoolRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "create_user_pool");
        log_debug << "Create user pool request, region:  " << request.region << " name: " << request.name;

        if (_database.UserPoolExists(request.region, request.name)) {
            log_error << "User pool exists already, region: " << request.region << " name: " << request.name;
            throw Core::ServiceException("User pool exists already, region: " + request.region + " name: " + request.name);
        }

        Dto::Cognito::CreateUserPoolResponse response{};
        try {
            std::string userPoolId = Core::AwsUtils::CreateCognitoUserPoolId(request.region);
            Database::Entity::Cognito::UserPool userPool = {
                    .region = request.region,
                    .userPoolId = userPoolId,
                    .name = request.name,
                    .arn = Core::AwsUtils::CreateCognitoUserPoolArn(request.region, _accountId, userPoolId),
            };

            userPool = _database.CreateUserPool(userPool);
            response = {{.requestId = request.requestId, .region = userPool.region}, userPool.name, userPool.arn, userPool.userPoolId};
            log_trace << "Create user pool outcome: " + response.ToJson();
            return response;

        } catch (Poco::Exception &ex) {
            log_error << "Create user pool request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void CognitoService::UpdateUserPool(const Dto::Cognito::UpdateUserPoolRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "update_user_pool");
        log_debug << "Create user pool request, region:  " << request.region << " userPoolId: " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exist, region: " << request.region << " userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exist, region: " + request.region + " userPoolId: " + request.userPoolId);
        }

        try {

            // Update user pool
            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            userPool.userPoolId = request.userPoolId;
            userPool.region = request.region;
            for (const auto &tag: request.tags) {
                userPool.tags[tag.first] = tag.second;
            }

            userPool = _database.UpdateUserPool(userPool);
            log_trace << "USer pool updated, userPoolId: " << userPool.userPoolId;

        } catch (Poco::Exception &ex) {
            log_error << "Create user pool request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::ListUserPoolResponse CognitoService::ListUserPools(const Dto::Cognito::ListUserPoolRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "list_user_pool");
        log_debug << "List user pools request, maxResults: " << request.maxResults;

        try {

            std::vector<Database::Entity::Cognito::UserPool> userPools = _database.ListUserPools(request.region);
            log_trace << "Got user pool list count: " << userPools.size();
            return Dto::Cognito::Mapper::map(request, userPools);

        } catch (Poco::Exception &ex) {
            log_error << "User pool list request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::DescribeUserPoolResponse CognitoService::DescribeUserPool(const Dto::Cognito::DescribeUserPoolRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "describe_user_pool");
        log_debug << "Describe user pool request, userPoolId: " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        try {

            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            log_trace << "Got user pool userPoolId: " << request.userPoolId;
            return Dto::Cognito::Mapper::map(request, userPool);

        } catch (Poco::Exception &ex) {
            log_error << "User pool list request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void CognitoService::DeleteUserPool(const Dto::Cognito::DeleteUserPoolRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "delete_user_pool");
        log_debug << "Delete user pool request, userPoolId:  " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        try {

            _database.DeleteUserPool(request.userPoolId);
            log_trace << "User pool deleted, userPoolId: " + request.userPoolId;

        } catch (Poco::Exception &ex) {
            log_error << "Create user pool request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::CreateUserPoolDomainResponse CognitoService::CreateUserPoolDomain(const Dto::Cognito::CreateUserPoolDomainRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "create_user_pool_domain");
        log_debug << "Create user pool request, region:  " << request.region << " name: " << request.domain;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exist, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exist, userPoolId: " + request.userPoolId);
        }

        try {

            // Get user pool
            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);

            // Update database
            userPool.domain = Dto::Cognito::Mapper::Mapper::map(request);
            userPool = _database.UpdateUserPool(userPool);

            Dto::Cognito::CreateUserPoolDomainResponse response{};
            response = {{.requestId = request.requestId, .region = userPool.region}, userPool.domain.domain};

            log_trace << "Create user pool domain result: " + response.ToJson();
            return response;

        } catch (Poco::Exception &ex) {
            log_error << "Create user pool request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::UpdateUserPoolDomainResponse CognitoService::UpdateUserPoolDomain(const Dto::Cognito::UpdateUserPoolDomainRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "update_user_pool_domain");
        log_debug << "Update user pool domain request, userPoolId: " << request.userPoolId << " domain: " << request.domain;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exist, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exist, userPoolId: " + request.userPoolId);
        }

        try {

            // Get user pool
            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            userPool.domain.domain = request.domain;

            // Update database
            userPool = _database.UpdateUserPool(userPool);
            log_trace << "User pool domain updated, userPoolId: " << userPool.userPoolId << " clientId: " << userPool.clientId;

            return {.cloudFrontDomain = userPool.domain.domain};

        } catch (Poco::Exception &ex) {
            log_error << "Update user pool domain request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::DescribeUserPoolDomainResponse CognitoService::DescribeUserPoolDomain(const Dto::Cognito::DescribeUserPoolDomainRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "describe_user_pool_domain");
        log_debug << "Describe user pool domain request, domain: " << request.domain;

        try {

            //Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            //log_trace << "Got user pool userPoolId: " << request.userPoolId;
            return {};

        } catch (Poco::Exception &ex) {
            log_error << "User pool list request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::CreateUserPoolClientResponse CognitoService::CreateUserPoolClient(const Dto::Cognito::CreateUserPoolClientRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "create_user_pool_client");
        log_debug << "Create user pool client request, clientName: " << request.clientName;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exist, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exist, userPoolId: " + request.userPoolId);
        }

        try {

            // Get user pool
            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);

            // Update database
            userPool.userPoolClients.emplace_back(Dto::Cognito::Mapper::Mapper::map(request));
            userPool = _database.UpdateUserPool(userPool);

            Dto::Cognito::CreateUserPoolClientResponse response{};
            response = {{.requestId = request.requestId, .region = userPool.region}, userPool.domain.domain};

            log_trace << "Create user pool client result: " + response.ToJson();
            return response;


        } catch (Poco::Exception &ex) {
            log_error << "Create user pool client request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::ListUserPoolClientsResponse CognitoService::ListUserPoolClients(const Dto::Cognito::ListUserPoolClientsRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "list_user_pool_clients");
        log_debug << "List user pools clients request, userPoolId: " << request.userPoolId << " maxResults: " << request.maxResults;

        try {

            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            log_trace << "Got user pool, region: " << userPool.region << " userPoolId: " << userPool.userPoolId;
            return Dto::Cognito::Mapper::map(request, userPool.userPoolClients);

        } catch (Poco::Exception &ex) {
            log_error << "User pool list request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::DescribeUserPoolClientResponse CognitoService::DescribeUserPoolClient(const Dto::Cognito::DescribeUserPoolClientRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "describe_user_pool_client");
        log_debug << "Describe user pool client request, userPoolId: " << request.userPoolId << " clientId: " << request.clientId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        try {

            std::string clientId = request.clientId;
            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            log_trace << "Got user pool userPoolId: " << request.userPoolId;
            auto it = find_if(userPool.userPoolClients.begin(),
                              userPool.userPoolClients.end(),
                              [clientId](const Database::Entity::Cognito::UserPoolClient &userPoolClient) {
                                  return userPoolClient.clientId == clientId;
                              });

            if (it != userPool.userPoolClients.end()) {
                Dto::Cognito::DescribeUserPoolClientResponse response;
                response.userPoolClient = Dto::Cognito::Mapper::map(*it);
                return response;
            }
            return {};

        } catch (Poco::Exception &ex) {
            log_error << "User pool list request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void CognitoService::UpdateUserPoolClient(const Dto::Cognito::UpdateUserPoolClientRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "update_user_pool_client");
        log_debug << "Update user pool client request, userPoolId: " << request.userPoolId << " clientId: " << request.clientId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exist, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exist, userPoolId: " + request.userPoolId);
        }

        try {

            // Get user pool
            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);

            std::string clientId = request.clientId;
            auto it = find_if(userPool.userPoolClients.begin(),
                              userPool.userPoolClients.end(),
                              [clientId](const Database::Entity::Cognito::UserPoolClient &userPoolClient) {
                                  return userPoolClient.clientId == clientId;
                              });

            // Update client
            if (it != userPool.userPoolClients.end()) {
                it->clientId = request.clientId;
                it->userPoolId = request.clientId;
                it->clientName = request.clientName;
                if (request.idTokenValidity > 0) {
                    it->idTokenValidity = Dto::Cognito::Mapper::GetValidityInSeconds(request.idTokenValidity, request.tokenValidityUnits.idToken);
                }
                if (request.accessTokenValidity > 0) {
                    it->accessTokenValidity = Dto::Cognito::Mapper::GetValidityInSeconds(request.accessTokenValidity, request.tokenValidityUnits.accessToken);
                }
                if (request.refreshTokenValidity > 0) {
                    it->refreshTokenValidity = Dto::Cognito::Mapper::GetValidityInSeconds(request.refreshTokenValidity, request.tokenValidityUnits.refreshToken);
                }

                // Update database
                userPool = _database.UpdateUserPool(userPool);
                log_trace << "User pool client updated, userPoolId: " << userPool.userPoolId << " clientId: " << userPool.clientId;
            }

        } catch (Poco::Exception &ex) {
            log_error << "Update user pool client request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void CognitoService::DeleteUserPoolClient(const Dto::Cognito::DeleteUserPoolClientRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "delete_user_pool_client");
        log_debug << "Delete user pool client request, userPoolId:  " << request.userPoolId << " clientId: " << request.clientId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        try {

            std::string clientId = request.clientId;
            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            const auto count = std::erase_if(userPool.userPoolClients, [clientId](const auto &item) {
                return item.clientId == clientId;
            });
            if (count == 0) {
                log_error << "User pool client does not exists, userPoolId: " << request.userPoolId << " clientId: " << request.clientId;
                throw Core::ServiceException("User pool client does not exists, userPoolId: " + request.userPoolId + " clientId: " + clientId);
            }
            userPool = _database.UpdateUserPool(userPool);
            log_trace << "User pool client deleted, userPoolId: " + request.userPoolId << " clients: " << userPool.userPoolClients.size();

        } catch (Poco::Exception &ex) {
            log_error << "Delete user pool client request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::AdminCreateUserResponse CognitoService::AdminCreateUser(const Dto::Cognito::AdminCreateUserRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "create_user");
        log_debug << "Admin create user request, userName:  " << request.userName << " userPoolId: " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (_database.UserExists(request.region, request.userPoolId, request.userName)) {
            throw Core::ServiceException("User exists exists already, userPoolId: " + request.userPoolId + " userName: " + request.userName);
        }

        try {
            Database::Entity::Cognito::User user = {
                    .region = request.region,
                    .userPoolId = request.userPoolId,
                    .userName = request.userName,
                    .enabled = true,
                    .confirmationCode = Core::AwsUtils::CreateCognitoConfirmationCode(),
                    .created = system_clock::now(),
                    .modified = system_clock::now(),
            };

            user = _database.CreateUser(user);
            Dto::Cognito::AdminCreateUserResponse response = {
                    {.region = user.region},
                    user.userName,
                    user.enabled};
            log_trace << "Create user response: " + response.ToJson();
            return response;

        } catch (Poco::Exception &ex) {
            log_error << "Create user request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void CognitoService::AdminAddUserToGroup(const Dto::Cognito::AdminAddUserToGroupRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "add_user_to_group");
        log_debug << "Admin add user to group request, request: " << request.ToString();

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (!_database.UserExists(request.region, request.userPoolId, request.userName)) {
            log_error << "User pool does not exists, userName: " << request.userName << " userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User does not exists, userName: " + request.userName + " userPoolId: " + request.userPoolId);
        }

        if (!_database.GroupExists(request.region, request.groupName)) {
            log_error << "Group does not exist, groupName: " << request.groupName << " userPoolId: " << request.userPoolId;
            throw Core::ServiceException("Group does not exist, groupName: " + request.groupName + " userPoolId: " + request.userPoolId);
        }

        try {
            Database::Entity::Cognito::User user = _database.GetUserByUserName(request.region, request.userPoolId, request.userName);
            Database::Entity::Cognito::Group group = _database.GetGroupByGroupName(request.region, request.userPoolId, request.groupName);

            if (!user.HasGroup(request.userPoolId, request.groupName)) {
                user.groups.emplace_back(group);
                user = _database.UpdateUser(user);
                log_debug << "Group added to user, userName: " << user.userName << " groupName: " << group.groupName;
            }

        } catch (Poco::Exception &ex) {
            log_error << "Create user request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::ListUsersResponse CognitoService::ListUsers(const Dto::Cognito::ListUsersRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "list_user");
        log_debug << "List users request, region: " << request.region << " userPoolId: " << request.userPoolId;

        Dto::Cognito::ListUsersResponse response{};

        try {

            Database::Entity::Cognito::UserList users = _database.ListUsers(request.region, request.userPoolId);
            response.users = users;
            log_trace << "Users list outcome: " + response.ToJson();
            return response;

        } catch (Poco::Exception &ex) {
            log_error << "User list request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::ListUsersInGroupResponse CognitoService::ListUsersInGroup(const Dto::Cognito::ListUsersInGroupRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "list_users_in_group");
        log_debug << "Admin add user to group request, request: " << request.ToString();

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (!_database.GroupExists(request.region, request.groupName)) {
            log_error << "Group does not exist, groupName: " << request.groupName << " userPoolId: " << request.userPoolId;
            throw Core::ServiceException("Group does not exist, groupName: " + request.groupName + " userPoolId: " + request.userPoolId);
        }

        try {

            Database::Entity::Cognito::UserList users = _database.ListUsersInGroup(request.region, request.userPoolId, request.groupName);

            return Dto::Cognito::Mapper::map(request, users);

        } catch (Poco::Exception &ex) {
            log_error << "Create user request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void CognitoService::AdminDeleteUser(const Dto::Cognito::AdminDeleteUserRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "delete_user");
        log_debug << "Admin delete user request, userName:  " << request.userName << " userPoolId: " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (!_database.UserExists(request.region, request.userPoolId, request.userName)) {
            throw Core::ServiceException("User does not exists, userPoolId: " + request.userPoolId + " userName: " + request.userName);
        }

        try {
            Database::Entity::Cognito::User user = _database.GetUserByUserName(request.region, request.userPoolId, request.userName);

            _database.DeleteUser(user);
            log_trace << "User deleted, userName:  " << request.userName << " userPoolId: " << request.userPoolId;

        } catch (Poco::Exception &ex) {
            log_error << "Delete user request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::CreateGroupResponse CognitoService::CreateGroup(const Dto::Cognito::CreateGroupRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "create_group");
        log_debug << "Create group request, region:  " << request.region << " name: " << request.groupName;

        if (_database.GroupExists(request.region, request.groupName)) {
            log_error << "User group exists already, region: " << request.region << " name: " << request.groupName;
            throw Core::ServiceException("User group exists already, region: " + request.region + " name: " + request.groupName);
        }

        Dto::Cognito::CreateGroupResponse response{};
        try {

            Database::Entity::Cognito::Group group = Dto::Cognito::Mapper::map(request);
            group = _database.CreateGroup(group);
            log_trace << "Cognito group created, group: " + response.ToJson();
            return Dto::Cognito::Mapper::map(request, group);

        } catch (Poco::Exception &ex) {
            log_error << "Create group request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::ListGroupsResponse CognitoService::ListGroups(const Dto::Cognito::ListGroupsRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "list_groups");
        log_debug << "List groups request, userPoolId: " << request.userPoolId << " maxResults: " << request.limit;

        try {

            std::vector<Database::Entity::Cognito::Group> groups = _database.ListGroups(request.region);
            log_trace << "Got groups list count: " << groups.size();
            return Dto::Cognito::Mapper::map(request, groups);

        } catch (Poco::Exception &ex) {
            log_error << "Group list request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    void CognitoService::DeleteGroup(const Dto::Cognito::DeleteGroupRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "delete_group");
        log_debug << "Delete group request, region:  " << request.region << " name: " << request.groupName;

        if (!_database.GroupExists(request.region, request.groupName)) {
            log_error << "Group does not exist, region: " << request.region << " name: " << request.groupName;
            throw Core::ServiceException("Group does not exist, region: " + request.region + " name: " + request.groupName);
        }

        try {

            _database.DeleteGroup(request.region, request.userPoolId, request.groupName);
            log_trace << "Cognito group deleted, group: " + request.ToJson();

        } catch (Poco::Exception &ex) {
            log_error << "Delete group request failed, message: " << ex.message();
            throw Core::ServiceException(ex.message());
        }
    }

    Dto::Cognito::SignUpResponse CognitoService::SignUp(const Dto::Cognito::SignUpRequest &request) {
        Dto::Cognito::SignUpResponse response;
        return response;
    }

    void CognitoService::ConfirmUser(const Dto::Cognito::AdminConfirmUserRequest &request) {
        Core::MetricServiceTimer measure(COGNITO_SERVICE_TIMER, "method", "confirm_user");
        log_debug << "Confirm user request, region:  " << request.region << " name: " << request.userName;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exist, region: " << request.region << " userPoolId: " << request.userPoolId;
            throw Core::NotFoundException("User pool does not exist, region: " + request.region + " userPoolId: " + request.userPoolId);
        }

        Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);

        if (_database.UserExists(request.region, userPool.userPoolId, request.userName)) {
            Database::Entity::Cognito::User user = _database.GetUserByUserName(request.region, userPool.userPoolId, request.userName);
            user.userStatus = Database::Entity::Cognito::UserStatus::CONFIRMED;
            user = _database.UpdateUser(user);
            log_debug << "User confirmed, userName: " << user.userName;
        } else {
            log_error << "User does not exist, region: " << request.region << " userPoolId: " << request.userPoolId;
            throw Core::NotFoundException("User does not exist, region: " + request.region + " userPoolId: " + request.userPoolId);
        }
    }
}// namespace AwsMock::Service
