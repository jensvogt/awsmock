
//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/cognito/CognitoService.h>

namespace AwsMock::Service {

    CognitoService::CognitoService() : _database(Database::CognitoDatabase::instance()) {
        _accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
    }

    Dto::Cognito::CreateUserPoolResponse CognitoService::CreateUserPool(const Dto::Cognito::CreateUserPoolRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "create_user_pool");
        log_debug << "Create user pool request, region:  " << request.region << " name: " << request.name;

        if (_database.UserPoolExists(request.region, request.name)) {
            log_error << "User pool exists already, region: " << request.region << " name: " << request.name;
            throw Core::ServiceException("User pool exists already, region: " + request.region + " name: " + request.name);
        }

        Dto::Cognito::CreateUserPoolResponse response{};
        try {
            // Create KMS SHA 256 key

            // Generate user pool ID
            const std::string userPoolId = Core::AwsUtils::CreateCognitoUserPoolId(request.region);
            Database::Entity::Cognito::UserPool userPool;
            userPool.region = request.region;
            userPool.userPoolId = userPoolId;
            userPool.name = request.name;
            userPool.arn = Core::AwsUtils::CreateCognitoUserPoolArn(request.region, _accountId, userPoolId);
            userPool = _database.CreateUserPool(userPool);
            response.requestId = request.requestId;
            response.region = userPool.region;
            response.userPool.name = userPool.name;
            response.userPool.arn = userPool.arn;
            response.userPool.userPoolId = userPool.userPoolId;
            log_trace << "Create user pool outcome: " + response.ToJson();
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void CognitoService::UpdateUserPool(const Dto::Cognito::UpdateUserPoolRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "update_user_pool");
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
            for (const auto &[fst, snd]: request.tags) {
                userPool.tags[fst] = snd;
            }

            userPool = _database.UpdateUserPool(userPool);
            log_trace << "User pool updated, userPoolId: " << userPool.userPoolId;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::ListUserPoolResponse CognitoService::ListUserPools(const Dto::Cognito::ListUserPoolRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "list_user_pool");
        log_debug << "List user pools request, maxResults: " << request.maxResults;

        try {
            const std::vector<Database::Entity::Cognito::UserPool> userPools = _database.ListUserPools(request.region);
            log_trace << "Got user pool list count: " << userPools.size();
            return Dto::Cognito::Mapper::map(request, userPools, request.maxResults);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::ListUserPoolCountersResponse CognitoService::ListUserPoolCounters(const Dto::Cognito::ListUserPoolCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "list_user_pool");
        log_debug << "List user pools counters request, pageSize: " << request.pageSize;

        try {
            const long total = _database.CountUserPools(request.region);
            const std::vector<Database::Entity::Cognito::UserPool> userPools = _database.ListUserPools(request.region, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            log_trace << "Got user pool list counters, count: " << userPools.size();
            Dto::Cognito::ListUserPoolCountersResponse response;
            response.userPoolCounters = Dto::Cognito::Mapper::mapCounter(userPools);
            response.total = total;
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::DescribeUserPoolResponse CognitoService::DescribeUserPool(const Dto::Cognito::DescribeUserPoolRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "describe_user_pool");
        log_debug << "Describe user pool request, userPoolId: " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        try {
            const Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            log_trace << "Got user pool userPoolId: " << request.userPoolId;
            return Dto::Cognito::Mapper::map(request, userPool);
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void CognitoService::DeleteUserPool(const Dto::Cognito::DeleteUserPoolRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "delete_user_pool");
        log_debug << "Delete user pool request, userPoolId:  " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        try {
            _database.DeleteUserPool(request.userPoolId);
            log_trace << "User pool deleted, userPoolId: " + request.userPoolId;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::CreateUserPoolDomainResponse CognitoService::CreateUserPoolDomain(const Dto::Cognito::CreateUserPoolDomainRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "create_user_pool_domain");
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
            response.requestId = request.requestId;
            response.region = userPool.region;
            response.cloudFrontDomain = userPool.domain.domain;

            log_trace << "Create user pool domain result: " + response.ToJson();
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::UpdateUserPoolDomainResponse CognitoService::UpdateUserPoolDomain(const Dto::Cognito::UpdateUserPoolDomainRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "update_user_pool_domain");
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
            log_trace << "User pool domain updated, userPoolId: " << userPool.userPoolId << " Domain: " << request.domain;
            Dto::Cognito::UpdateUserPoolDomainResponse response{};
            response.requestId = request.requestId;
            response.user = request.user;
            response.region = userPool.region;
            response.cloudFrontDomain = userPool.domain.domain;
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::DescribeUserPoolDomainResponse CognitoService::DescribeUserPoolDomain(const Dto::Cognito::DescribeUserPoolDomainRequest &request) {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "describe_user_pool_domain");
        log_debug << "Describe user pool domain request, domain: " << request.domain;

        try {
            //Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            //log_trace << "Got user pool userPoolId: " << request.userPoolId;
            return {};
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::CreateUserPoolClientResponse CognitoService::CreateUserPoolClient(const Dto::Cognito::CreateUserPoolClientRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "create_user_pool_client");
        log_debug << "Create user pool client request, clientName: " << request.clientName;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exist, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exist, userPoolId: " + request.userPoolId);
        }

        try {
            // Get user pool
            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);

            // Create client
            Database::Entity::Cognito::UserPoolClient userPoolClient = Dto::Cognito::Mapper::Mapper::map(request);

            // Create KMS symmetric key if needed
            if (userPoolClient.generateSecret) {
                userPoolClient.clientSecret = Core::StringUtils::GenerateRandomAlphanumericString(64);
            }

            // Update database
            userPool.userPoolClients.emplace_back(userPoolClient);
            userPool = _database.UpdateUserPool(userPool);

            Dto::Cognito::CreateUserPoolClientResponse response{};
            response.requestId = request.requestId;
            response.region = userPool.region;
            response.userGroupClient.clientId = userPoolClient.clientId;
            log_trace << "Create user pool client result: " + response.ToJson();
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::ListUserPoolClientsResponse CognitoService::ListUserPoolClients(const Dto::Cognito::ListUserPoolClientsRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "list_user_pool_clients");
        log_debug << "List user pools clients request, userPoolId: " << request.userPoolId << " maxResults: " << request.maxResults;

        try {
            const Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            log_trace << "Got user pool, region: " << userPool.region << " userPoolId: " << userPool.userPoolId;
            return Dto::Cognito::Mapper::map(request, userPool.userPoolClients);
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::DescribeUserPoolClientResponse CognitoService::DescribeUserPoolClient(const Dto::Cognito::DescribeUserPoolClientRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "describe_user_pool_client");
        log_debug << "Describe user pool client request, userPoolId: " << request.userPoolId << " clientId: " << request.clientId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        try {
            std::string clientId = request.clientId;
            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            log_trace << "Got user pool userPoolId: " << request.userPoolId;
            auto it = std::ranges::find_if(userPool.userPoolClients,
                                           [clientId](const Database::Entity::Cognito::UserPoolClient &userPoolClient) {
                                               return userPoolClient.clientId == clientId;
                                           });

            if (it != userPool.userPoolClients.end()) {
                Dto::Cognito::DescribeUserPoolClientResponse response;
                response.userPoolClient = Dto::Cognito::Mapper::map(*it);
                return response;
            }
            return {};
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void CognitoService::UpdateUserPoolClient(const Dto::Cognito::UpdateUserPoolClientRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "update_user_pool_client");
        log_debug << "Update user pool client request, userPoolId: " << request.userPoolId << " clientId: " << request.clientId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exist, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exist, userPoolId: " + request.userPoolId);
        }

        try {
            // Get user pool
            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);

            std::string clientId = request.clientId;
            const auto it = std::ranges::find_if(userPool.userPoolClients,
                                                 [clientId](
                                                 const Database::Entity::Cognito::UserPoolClient &userPoolClient) {
                                                     return userPoolClient.clientId == clientId;
                                                 });

            // Update client
            if (it != userPool.userPoolClients.end()) {
                it->clientId = request.clientId;
                it->userPoolId = request.clientId;
                it->clientName = request.clientName;
                if (request.idTokenValidity > 0) {
                    it->idTokenValidity = Dto::Cognito::Mapper::GetValidityInSeconds(
                            request.idTokenValidity,
                            request.tokenValidityUnits.idToken);
                }
                if (request.accessTokenValidity > 0) {
                    it->accessTokenValidity = Dto::Cognito::Mapper::GetValidityInSeconds(
                            request.accessTokenValidity,
                            request.tokenValidityUnits.accessToken);
                }
                if (request.refreshTokenValidity > 0) {
                    it->refreshTokenValidity = Dto::Cognito::Mapper::GetValidityInSeconds(
                            request.refreshTokenValidity,
                            request.tokenValidityUnits.refreshToken);
                }

                // Update database
                userPool = _database.UpdateUserPool(userPool);
                log_trace << "User pool client updated, userPoolId: " << userPool.userPoolId << " clientId: " << request.clientId;
            }
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void CognitoService::DeleteUserPoolClient(const Dto::Cognito::DeleteUserPoolClientRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "delete_user_pool_client");
        log_debug << "Delete user pool client request, userPoolId:  " << request.userPoolId << " clientId: " << request.clientId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        try {
            std::string clientId = request.clientId;
            Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            const auto count = std::erase_if(userPool.userPoolClients,
                                             [clientId](const auto &item) {
                                                 return item.clientId == clientId;
                                             });
            if (count == 0) {
                log_error << "User pool client does not exists, userPoolId: " << request.userPoolId << " clientId: " << request.clientId;
                throw Core::ServiceException(
                        "User pool client does not exists, userPoolId: " + request.userPoolId + " clientId: " + clientId);
            }
            userPool = _database.UpdateUserPool(userPool);
            log_trace << "User pool client deleted, userPoolId: " + request.userPoolId << " clients: " << userPool.userPoolClients.size();
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::AdminCreateUserResponse CognitoService::AdminCreateUser(const Dto::Cognito::AdminCreateUserRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "create_user");
        log_debug << "Admin create user request, userName:  " << request.userName << " userPoolId: " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (_database.UserExists(request.region, request.userPoolId, request.userName)) {
            log_error << "User exists exists already, userPoolId: " << request.userPoolId << ", userName: " + request.userName;
            throw Core::ServiceException("User exists exists already, userPoolId: " + request.userPoolId + " userName: " + request.userName);
        }

        try {
            Database::Entity::Cognito::User user;
            user.region = request.region;
            user.userPoolId = request.userPoolId;
            user.userName = request.userName;
            user.enabled = true;
            user.userStatus = Database::Entity::Cognito::UserStatus::UNCONFIRMED;
            user.password = Core::StringUtils::GenerateRandomPassword(12);
            user.confirmationCode = Core::AwsUtils::CreateCognitoConfirmationCode();
            user = _database.CreateUser(user);

            Dto::Cognito::AdminCreateUserResponse response;
            response.region = user.region;
            response.userName = user.userName;
            response.enabled = user.enabled;
            log_trace << "Create user response: " + response.ToJson();
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::AdminGetUserResponse CognitoService::AdminGetUser(const Dto::Cognito::AdminGetUserRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "admin_get_user");
        log_debug << "Admin get user request, userName:  " << request.userName << " userPoolId: " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::BadRequestException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (!_database.UserExists(request.region, request.userPoolId, request.userName)) {
            log_error << "User does not exists, userPoolId: " << request.userPoolId << ", userName: " + request.userName;
            throw Core::BadRequestException("User does not exists, userPoolId: " + request.userPoolId + " userName: " + request.userName);
        }

        try {

            Database::Entity::Cognito::User user = _database.GetUserByUserName(request.region, request.userPoolId, request.userName);
            Dto::Cognito::AdminGetUserResponse response;
            response.id = user.oid;
            response.region = user.region;
            response.userPoolId = user.userPoolId;
            response.userName = user.userName;
            response.password = user.password;
            response.enabled = user.enabled;
            response.userStatus = user.userStatus;
            response.created = user.created;
            response.modified = user.modified;
            log_trace << "Get admin user response: " + response.ToJson();
            return response;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void CognitoService::AdminAddUserToGroup(const Dto::Cognito::AdminAddUserToGroupRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "add_user_to_group");
        log_debug << "Admin add user to group request, request: " << request;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (!_database.UserExists(request.region, request.userPoolId, request.userName)) {
            log_error << "User does not exists, userName: " << request.userName << " userPoolId: " << request.userPoolId;
            throw Core::ServiceException(
                    "User does not exists, userName: " + request.userName + " userPoolId: " + request.userPoolId);
        }

        if (!_database.GroupExists(request.region, request.groupName)) {
            log_error << "Group does not exist, groupName: " << request.groupName << " userPoolId: " << request.userPoolId;
            throw Core::ServiceException(
                    "Group does not exist, groupName: " + request.groupName + " userPoolId: " + request.userPoolId);
        }

        try {
            Database::Entity::Cognito::User user = _database.GetUserByUserName(
                    request.region,
                    request.userPoolId,
                    request.userName);
            Database::Entity::Cognito::Group group = _database.GetGroupByGroupName(
                    request.region,
                    request.userPoolId,
                    request.groupName);

            if (!user.HasGroup(request.userPoolId, request.groupName)) {
                user.groups.emplace_back(group);
                user = _database.UpdateUser(user);
                log_debug << "Group added to user, userName: " << user.userName << " groupName: " << group.groupName;
            }
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void CognitoService::AdminRemoveUserFromGroup(const Dto::Cognito::AdminRemoveUserFromGroupRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "add_user_to_group");
        log_debug << "Admin add user to group request, request: " << request;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (!_database.UserExists(request.region, request.userPoolId, request.userName)) {
            log_error << "User does not exists, userName: " << request.userName << " userPoolId: " << request.userPoolId;
            throw Core::ServiceException(
                    "User does not exists, userName: " + request.userName + " userPoolId: " + request.userPoolId);
        }

        if (!_database.GroupExists(request.region, request.groupName)) {
            log_error << "Group does not exist, groupName: " << request.groupName << " userPoolId: " << request.userPoolId;
            throw Core::ServiceException(
                    "Group does not exist, groupName: " + request.groupName + " userPoolId: " + request.userPoolId);
        }

        try {
            Database::Entity::Cognito::User user = _database.GetUserByUserName(
                    request.region,
                    request.userPoolId,
                    request.userName);
            Database::Entity::Cognito::Group group = _database.GetGroupByGroupName(
                    request.region,
                    request.userPoolId,
                    request.groupName);

            if (user.HasGroup(request.userPoolId, request.groupName)) {
                std::string groupName = group.groupName;
                std::erase_if(user.groups,
                              [&groupName](const Database::Entity::Cognito::Group &g) {
                                  return groupName == g.groupName;
                              });
                user = _database.UpdateUser(user);
                log_debug << "Group added to user, userName: " << user.userName << " groupName: " << group.groupName;
            }
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::ListUsersResponse CognitoService::ListUsers(const Dto::Cognito::ListUsersRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "list_user");
        log_debug << "List users request, region: " << request.region << " userPoolId: " << request.userPoolId;

        Dto::Cognito::ListUsersResponse response{};

        try {
            const long total = _database.CountUsers(request.region, request.userPoolId);
            const Database::Entity::Cognito::UserList users = _database.ListUsers(request.region, request.userPoolId);
            response.users = Dto::Cognito::Mapper::map(users);
            response.total = total;

            log_trace << "Users list outcome: " + response.ToJson();
            return response;
        } catch (bsoncxx::exception &ex) {
            log_error << "User list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Cognito::ListUserCountersResponse CognitoService::ListUserCounters(const Dto::Cognito::ListUserCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "list_user_counters");
        log_debug << "List user counters request, pageSize: " << request.pageSize;

        try {
            const long total = _database.CountUsers(request.region, request.userPoolId);
            const std::vector<Database::Entity::Cognito::User> users = _database.ListUsers(request.region, request.userPoolId, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));
            log_trace << "Got user list counters, count: " << users.size();
            Dto::Cognito::ListUserCountersResponse response;
            response.users = Dto::Cognito::Mapper::map(users);
            response.total = total;
            return response;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Dto::Cognito::ListUsersInGroupResponse CognitoService::ListUsersInGroup(const Dto::Cognito::ListUsersInGroupRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "list_users_in_group");
        log_debug << "Admin add user to group request, request: " << request;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (!_database.GroupExists(request.region, request.groupName)) {
            log_error << "Group does not exist, groupName: " << request.groupName << " userPoolId: " << request.userPoolId;
            throw Core::ServiceException("Group does not exist, groupName: " + request.groupName + " userPoolId: " + request.userPoolId);
        }

        try {
            const Database::Entity::Cognito::UserList users = _database.ListUsersInGroup(
                    request.region,
                    request.userPoolId,
                    request.groupName);

            return Dto::Cognito::Mapper::map(request, users);
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void CognitoService::AdminEnableUser(const Dto::Cognito::AdminEnableUserRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "enable_user");
        log_debug << "Admin enable user request, userName:  " << request.userName << " userPoolId: " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (!_database.UserExists(request.region, request.userPoolId, request.userName)) {
            log_error << "User does not exists, userPoolId: " << request.userPoolId << " userName: " << request.userName;
            throw Core::ServiceException("User does not exists, userPoolId: " + request.userPoolId + " userName: " + request.userName);
        }

        try {
            Database::Entity::Cognito::User user = _database.GetUserByUserName(
                    request.region,
                    request.userPoolId,
                    request.userName);
            user.enabled = true;
            user = _database.UpdateUser(user);
            log_trace << "User enabled, userName:  " << user.userName << " userPoolId: " << user.userPoolId;
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void CognitoService::AdminDisableUser(const Dto::Cognito::AdminDisableUserRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "disable_user");
        log_debug << "Admin disable user request, userName:  " << request.userName << " userPoolId: " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (!_database.UserExists(request.region, request.userPoolId, request.userName)) {
            log_error << "User does not exists, userPoolId: " << request.userPoolId << " userName: " << request.userName;
            throw Core::ServiceException("User does not exists, userPoolId: " + request.userPoolId + " userName: " + request.userName);
        }

        try {
            Database::Entity::Cognito::User user = _database.GetUserByUserName(request.region, request.userPoolId, request.userName);
            user.enabled = false;
            user = _database.UpdateUser(user);
            log_trace << "User disabled, userName:  " << user.userName << " userPoolId: " << user.userPoolId;
        } catch (bsoncxx::exception &ex) {
            log_error << "Disable user request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void CognitoService::AdminDeleteUser(const Dto::Cognito::AdminDeleteUserRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "delete_user");
        log_debug << "Admin delete user request, userName:  " << request.userName << " userPoolId: " << request.userPoolId;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exists, userPoolId: " << request.userPoolId;
            throw Core::ServiceException("User pool does not exists, userPoolId: " + request.userPoolId);
        }

        if (!_database.UserExists(request.region, request.userPoolId, request.userName)) {
            log_error << "User does not exists, userPoolId: " << request.userPoolId << " userName: " << request.userName;
            throw Core::ServiceException("User does not exists, userPoolId: " + request.userPoolId + " userName: " + request.userName);
        }

        try {
            const Database::Entity::Cognito::User user = _database.GetUserByUserName(request.region, request.userPoolId, request.userName);

            _database.DeleteUser(user);
            log_trace << "User deleted, userName:  " << request.userName << " userPoolId: " << request.userPoolId;
        } catch (bsoncxx::exception &ex) {
            log_error << "Delete user request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Cognito::CreateGroupResponse
    CognitoService::CreateGroup(const Dto::Cognito::CreateGroupRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "create_group");
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
        } catch (bsoncxx::exception &ex) {
            log_error << "Create group request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Cognito::ListGroupsResponse CognitoService::ListGroups(const Dto::Cognito::ListGroupsRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "list_groups");
        log_debug << "List groups request, userPoolId: " << request.userPoolId << " maxResults: " << request.limit;

        try {
            const std::vector<Database::Entity::Cognito::Group> groups = _database.ListGroups(request.region);
            log_trace << "Got groups list count: " << groups.size();
            return Dto::Cognito::Mapper::map(request, groups);
        } catch (bsoncxx::exception &ex) {
            log_error << "Group list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void CognitoService::DeleteGroup(const Dto::Cognito::DeleteGroupRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "delete_group");
        log_debug << "Delete group request, region:  " << request.region << " name: " << request.groupName;

        if (!_database.GroupExists(request.region, request.groupName)) {
            log_error << "Group does not exist, region: " << request.region << " name: " << request.groupName;
            throw Core::ServiceException("Group does not exist, region: " + request.region + " name: " + request.groupName);
        }

        try {
            _database.DeleteGroup(request.region, request.userPoolId, request.groupName);
            log_trace << "Cognito group deleted, group: " << request.ToString();

        } catch (bsoncxx::exception &ex) {
            log_error << "Delete group request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Cognito::SignUpResponse CognitoService::SignUp(const Dto::Cognito::SignUpRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "signup_user");
        log_debug << "Signup user request, region:  " << request.region << " userName: " << request.userName << " clientId: " << request.clientId;

        if (_database.UserExists(request.region, request.userName)) {
            log_error << "User exists exists already, userName: " << request.userName;
            throw Core::ServiceException("User exists exists already, userName: " + request.userName);
        }

        Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByClientId(request.clientId);
        try {
            Database::Entity::Cognito::User user;
            user.region = request.region;
            user.userPoolId = userPool.userPoolId;
            user.userName = request.userName;
            user.enabled = true;
            user.confirmationCode = Core::AwsUtils::CreateCognitoConfirmationCode();

            user = _database.CreateUser(user);
            Dto::Cognito::SignUpResponse response;
            response.userConfirmed = false;
            response.region = user.region;
            response.requestId = request.requestId;
            log_trace << "Signup user response: " + response.ToJson();
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "Create user request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void CognitoService::ConfirmUser(const Dto::Cognito::AdminConfirmUserRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "confirm_user");
        log_debug << "Confirm user request, region:  " << request.region << " name: " << request.userName;

        if (!_database.UserPoolExists(request.userPoolId)) {
            log_error << "User pool does not exist, region: " << request.region << " userPoolId: " << request.userPoolId;
            throw Core::NotFoundException("User pool does not exist, region: " + request.region + " userPoolId: " + request.userPoolId);
        }

        if (_database.UserExists(request.region, request.userPoolId, request.userName)) {
            const Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByUserPoolId(request.userPoolId);
            Database::Entity::Cognito::User user = _database.GetUserByUserName(
                    request.region,
                    userPool.userPoolId,
                    request.userName);
            user.userStatus = Database::Entity::Cognito::UserStatus::CONFIRMED;
            user = _database.UpdateUser(user);
            log_debug << "User confirmed, userName: " << user.userName;
        } else {
            log_error << "User does not exist, region: " << request.region << " userPoolId: " << request.userPoolId;
            throw Core::NotFoundException(
                    "User does not exist, region: " + request.region + " userPoolId: " + request.userPoolId);
        }
    }

    Dto::Cognito::InitiateAuthResponse CognitoService::InitiateAuth(Dto::Cognito::InitiateAuthRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "initiate_auth");
        log_debug << "Confirm initiate auth request, region:  " << request.region << " clientId: " << request.clientId;

        if (!_database.ClientIdExists(request.region, request.clientId)) {
            log_error << "Client id does not exist, region: " << request.region << " clientId: " << request.clientId;
            throw Core::NotFoundException("Client id does not exist, region: " + request.region + " clientId: " + request.clientId);
        }

        std::string tmp = request.GetUserId();
        if (!_database.UserExists(request.region, request.GetUserId())) {
            log_error << "User does not exist, region: " << request.region << " user: " << request.GetUserId();
            throw Core::NotFoundException("User does not exist, region: " + request.region + " user: " + request.GetUserId());
        }

        Database::Entity::Cognito::UserPool userPool = _database.GetUserPoolByClientId(request.clientId);
        const Database::Entity::Cognito::UserPoolClient userPoolClient = userPool.GetClient(request.clientId);

        // Verify A
        Core::SrpUtils srpUtils;
        if (!srpUtils.VerifyA(request.authParameters.at("SRP_A"))) {
            log_error << "Failed to verify user spr auth request: " << request.authParameters.at("SRP_A");
            throw Core::UnauthorizedException("Failed to verify user spr auth request: " + request.authParameters.at("SRP_A"));
        }

        BIGNUM *salt = BN_new();
        BIGNUM *v = BN_new();
        std::string saltStr = "BEB25379D1A8581EB5A727673A2441EE";
        BN_hex2bn(&salt, "BEB25379D1A8581EB5A727673A2441EE");
        BN_hex2bn(&v, "7E273DE8696FFC4F4E337D05B4B375BEB0DDE1569E8FA00A9886D8129BADA1F1822223CA1A605B530E379BA4729FDC59F105B4787E5186F5C671085A1447B52A48CF1970B4FB6F8400BBF4CEBFBB168152E08AB5EA53D15C1AFF87B2B9DA6E04E058AD51CC72BFC9033B564E26480D78E955A5E29E7AB245DB2BE315E2099AFB");
        srpUtils.SetSaltAndV(salt, v);
        const BIGNUM *srpB = srpUtils.CalcB();
        //  BN_print_fp(stderr, srpB);
        std::string srpBStr = BN_bn2hex(srpB);

        Dto::Cognito::InitiateAuthResponse response;
        response.region = request.region;
        response.user = request.user;
        response.clientId = request.clientId;
        response.session = Core::StringUtils::CreateRandomUuid();
        response.availableChallenges.emplace_back("PASSWORD_VERIFIER_CHALLENGE");
        response.availableChallenges.emplace_back("PASSWORD_SRP");
        response.challengeName = "PASSWORD_VERIFIER_CHALLENGE";
        response.challengeParameters = {
                {"SRP_B", srpBStr},
                {"USERNAME", request.authParameters.at("USERNAME")},
                {"SALT", saltStr},
                {"USER_ID_FOR_SRP", request.authParameters.at("USERNAME")},
                {"SECRET_BLOCK", Core::StringUtils::GenerateRandomString(40)}};
        log_info << response.ToJson();
        return response;
    }

    Dto::Cognito::RespondToAuthChallengeResponse CognitoService::RespondToAuthChallenge(Dto::Cognito::RespondToAuthChallengeRequest &request) const {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "respond_to_auth_challenge");
        log_debug << "Respond to auth challenge request, region:  " << request.region << " clientId: " << request.clientId;

        if (!_database.ClientIdExists(request.region, request.clientId)) {
            log_error << "Client id does not exist, region: " << request.region << " clientId: " << request.clientId;
            throw Core::NotFoundException("Client id does not exist, region: " + request.region + " clientId: " + request.clientId);
        }

        Dto::Cognito::RespondToAuthChallengeResponse response;
        response.challengeName = request.challengeName;
        response.requestId = request.requestId;
        response.authenticationResult.accessToken = Core::JwtUtils::CreateTokenHs256(
                request.GetPasswordClaim_Signature(),
                request.GetUserName(),
                {});
        response.authenticationResult.refreshToken = Core::JwtUtils::CreateTokenHs256(
                request.GetPasswordClaim_Signature(),
                request.GetUserName(),
                {});
        response.authenticationResult.idToken = Core::JwtUtils::CreateTokenHs256(
                request.GetPasswordClaim_Signature(),
                request.GetUserName(),
                {});
        response.session = request.session;
        return response;
    }

    void CognitoService::GlobalSignOut(const Dto::Cognito::GlobalSignOutRequest &request) {
        Monitoring::MonitoringTimer measure(COGNITO_SERVICE_TIMER, COGNITO_SERVICE_COUNTER, "action", "global_sign_out");
        log_debug << "Global sign out request, region:  " << request.region << " accessToken: " << request.accessToken;
    }
}// namespace AwsMock::Service
