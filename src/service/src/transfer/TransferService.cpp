//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/service/transfer/TransferService.h>

namespace AwsMock::Service {

    Dto::Transfer::CreateServerResponse TransferService::CreateTransferServer(Dto::Transfer::CreateServerRequest &request) const {
        Monitoring::MetricServiceTimer measure(TRANSFER_SERVICE_TIMER, "method", "create_transfer_server");
        log_debug << "Create transfer manager";

        // Check existence
        // TODO: How can see that a server exists already
        if (_transferDatabase.TransferExists(request.region, ProtocolTypeToString(request.protocols[0]))) {
            log_error << "Transfer manager exists already";
            throw Core::ServiceException("Transfer manager exists already");
        }

        std::string serverId = "s-" + Core::StringUtils::ToLower(Core::StringUtils::GenerateRandomHexString(20));

        Database::Entity::Transfer::Transfer transferEntity;
        std::string accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");
        std::string transferArn = Core::AwsUtils::CreateTransferArn(request.region, accountId, serverId);
        std::string listenAddress = Core::Configuration::instance().GetValueString("awsmock.modules.transfer.ftp.address");

        // Create entity
        int ftpPort = Core::Configuration::instance().GetValueInt("awsmock.modules.transfer.ftp.port");
        transferEntity = {.region = request.region, .serverId = serverId, .arn = transferArn, .ports = {ftpPort}, .listenAddress = listenAddress};
        transferEntity.protocols.emplace_back(ProtocolTypeToString(request.protocols[0]));

        // Add anonymous user
        Database::Entity::Transfer::User anonymousUser = {.userName = "anonymous", .password = "secret", .homeDirectory = "/"};
        transferEntity.users.emplace_back(anonymousUser);

        transferEntity = _transferDatabase.CreateTransfer(transferEntity);

        // Create response
        Dto::Transfer::CreateServerResponse response{.region = transferEntity.region, .serverId = serverId, .arn = transferArn};
        log_error << "Transfer manager created, address: " << listenAddress << " port: " << ftpPort;

        return response;
    }

    Dto::Transfer::CreateUserResponse TransferService::CreateUser(Dto::Transfer::CreateUserRequest &request) const {
        Monitoring::MetricServiceTimer measure(TRANSFER_SERVICE_TIMER, "method", "create_user_server");
        log_debug << "Create user request";

        Database::Entity::Transfer::Transfer transferEntity;

        if (!_transferDatabase.TransferExists(request.region, request.serverId)) {

            throw Core::ServiceException("Transfer manager with ID '" + request.serverId + "  does not exist");
        }
        transferEntity = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

        // Check user
        if (transferEntity.HasUser(request.userName)) {
            log_warning << "Transfer manager has already a user with name '" + request.userName + "'";
            Database::Entity::Transfer::User user = transferEntity.GetUser(request.userName);
            return {.region = transferEntity.region, .serverId = transferEntity.serverId, .userName = user.userName};
        }

        // Get home directory
        std::string homeDirectory = request.userName;
        if (!Core::StringUtils::IsNullOrEmpty(&request.homeDirectory)) {
            homeDirectory = request.homeDirectory;
        }

        // Add user
        std::string accountId = Core::Configuration::instance().GetValueString("awsmock.access.account-id");
        std::string userArn = Core::AwsUtils::CreateTransferUserArn(request.region, accountId, transferEntity.serverId, request.userName);
        Database::Entity::Transfer::User user = {
                .userName = request.userName,
                .password = Core::StringUtils::GenerateRandomPassword(8),
                .homeDirectory = homeDirectory,
                .arn = userArn};
        transferEntity.users.emplace_back(user);

        // Update database
        transferEntity = _transferDatabase.UpdateTransfer(transferEntity);
        log_debug << "Updated transfer manager, serverId: " << transferEntity.serverId;

        // Create response
        Dto::Transfer::CreateUserResponse response{.region = transferEntity.region, .serverId = transferEntity.serverId, .userName = request.userName};

        return response;
    }

    Dto::Transfer::ListServerResponse TransferService::ListServers(const Dto::Transfer::ListServerRequest &request) const {
        Monitoring::MetricServiceTimer measure(TRANSFER_SERVICE_TIMER, "method", "list_servers");

        try {
            std::vector<Database::Entity::Transfer::Transfer> servers = _transferDatabase.ListServers(request.region);

            auto response = Dto::Transfer::ListServerResponse();
            response.nextToken = Core::StringUtils::CreateRandomUuid();
            for (const auto &s: servers) {
                Dto::Transfer::Server server = {
                        .arn = s.arn,
                        .serverId = s.serverId,
                        .state = ServerStateToString(s.state),
                        .userCount = static_cast<int>(s.users.size())};
                response.servers.emplace_back(server);
            }

            log_trace << "Transfer server list outcome: " + response.ToJson();
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "Transfer server list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Transfer::ListServerCountersResponse TransferService::ListServerCounters(const Dto::Transfer::ListServerCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(TRANSFER_SERVICE_TIMER, "method", "list_server_counters");

        try {
            std::vector<Database::Entity::Transfer::Transfer> servers = _transferDatabase.ListServers(request.region, request.prefix, request.pageSize, request.pageIndex, request.sortColumns);

            auto response = Dto::Transfer::ListServerCountersResponse();
            response.total = _transferDatabase.CountServers(request.region);
            for (const auto &s: servers) {
                Dto::Transfer::Server server = {
                        .arn = s.arn,
                        .serverId = s.serverId,
                        .state = ServerStateToString(s.state),
                        .userCount = static_cast<int>(s.users.size()),
                        .lastStarted = s.lastStarted,
                        .created = s.created,
                        .modified = s.modified,
                };
                response.transferServers.emplace_back(server);
            }

            log_trace << "Transfer server list outcome: " + response.ToJson();
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "Transfer server list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Transfer::GetServerDetailsResponse TransferService::GetServerDetails(const Dto::Transfer::GetServerDetailsRequest &request) const {
        Monitoring::MetricServiceTimer measure(TRANSFER_SERVICE_TIMER, "method", "get_server_details");

        // Check existence
        if (!_transferDatabase.TransferExists(request.region, request.serverId)) {
            log_error << "Transfer server does not exist, region: " << request.region << " serverId: " << request.serverId;
            throw Core::ServiceException("Transfer server does not exist, region: " + request.region + " serverId: " + request.serverId);
        }

        try {
            Database::Entity::Transfer::Transfer server = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

            auto response = Dto::Transfer::GetServerDetailsResponse();
            response.server = {
                    .region = server.region,
                    .arn = server.arn,
                    .serverId = server.serverId,
                    .state = ServerStateToString(server.state),
                    .userCount = static_cast<int>(server.users.size()),
                    .ports = server.ports,
                    .concurrency = server.concurrency,
                    .lastStarted = server.lastStarted,
                    .created = server.created,
                    .modified = server.modified,
            };

            log_trace << "Transfer server list outcome: " + response.ToJson();
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "Transfer server list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Transfer::ListUsersResponse TransferService::ListUsers(const Dto::Transfer::ListUsersRequest &request) const {
        Monitoring::MetricServiceTimer measure(TRANSFER_SERVICE_TIMER, "method", "list_users");

        try {
            const std::vector<Database::Entity::Transfer::User> users = _transferDatabase.ListUsers(request.region, request.serverId);

            Dto::Transfer::ListUsersResponse response = Dto::Transfer::Mapper::map(request, users);
            response.nextToken = Core::StringUtils::CreateRandomUuid();

            log_trace << "User list result: " << response.ToJson();
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "User list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }
    Dto::Transfer::ListUserCountersResponse TransferService::ListUserCounters(const Dto::Transfer::ListUserCountersRequest &request) const {
        Monitoring::MetricServiceTimer measure(TRANSFER_SERVICE_TIMER, "method", "list_user_counters");

        try {
            std::vector<Database::Entity::Transfer::User> users = _transferDatabase.ListUsers(request.region, request.serverId, request.prefix, request.pageSize, request.pageIndex, request.sortColumns);

            auto response = Dto::Transfer::ListUserCountersResponse();
            response.total = _transferDatabase.CountUsers(request.region, request.serverId);
            for (const auto &user: users) {
                Dto::Transfer::User userDto = {
                        .region = request.region,
                        .userName = user.userName,
                        .arn = user.arn,
                        .password = user.password};
                response.userCounters.emplace_back(userDto);
            }

            log_trace << "Transfer user list result: " << response.ToJson();
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "Transfer user list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void TransferService::StartServer(const Dto::Transfer::StartServerRequest &request) const {
        Monitoring::MetricServiceTimer measure(TRANSFER_SERVICE_TIMER, "method", "start_server");

        Database::Entity::Transfer::Transfer server;
        try {
            if (!_transferDatabase.TransferExists(request.region, request.serverId)) {
                log_error << "Handler with ID '" << request.serverId << "' does not exist";
                throw Core::ServiceException("Handler with ID '" + request.serverId + "' does not exist");
            }

            // Get the manager
            server = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

            // Update state, rest will be done by transfer worker
            server.state = Database::Entity::Transfer::ServerState::ONLINE;
            server = _transferDatabase.UpdateTransfer(server);
            log_info << "Transfer server started, serverId: " << server.serverId;

        } catch (bsoncxx::exception &ex) {

            // Update state
            server.state = Database::Entity::Transfer::ServerState::START_FAILED;
            server = _transferDatabase.UpdateTransfer(server);

            log_error << "Start transfer server request failed, serverId: " << server.serverId << " message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void TransferService::StopServer(const Dto::Transfer::StopServerRequest &request) const {
        Monitoring::MetricServiceTimer measure(TRANSFER_SERVICE_TIMER, "method", "stop_server");

        Database::Entity::Transfer::Transfer server;
        try {
            if (!_transferDatabase.TransferExists(request.region, request.serverId)) {
                throw Core::ServiceException("Handler with ID '" + request.serverId + "' does not exist");
            }

            // Get the manager
            server = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

            // Update state, rest will be done by transfer worker
            server.state = Database::Entity::Transfer::ServerState::OFFLINE;
            server = _transferDatabase.UpdateTransfer(server);
            log_info << "Transfer manager stopped, serverId: " << server.serverId;

        } catch (bsoncxx::exception &ex) {

            // Update state
            server.state = Database::Entity::Transfer::ServerState::STOP_FAILED;
            server = _transferDatabase.UpdateTransfer(server);

            log_error << "Stop manager request failed, serverId: " << server.serverId << " message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void TransferService::DeleteServer(const Dto::Transfer::DeleteServerRequest &request) const {
        Monitoring::MetricServiceTimer measure(TRANSFER_SERVICE_TIMER, "method", "delete_server");

        Database::Entity::Transfer::Transfer server;
        try {
            if (!_transferDatabase.TransferExists(request.region, request.serverId)) {
                log_error << "Handler with ID '" + request.serverId + "' does not exist";
                throw Core::ServiceException("Handler with ID '" + request.serverId + "' does not exist");
            }

            // Get the manager
            server = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

            // Update state, rest will be done by transfer worker
            server.state = Database::Entity::Transfer::ServerState::OFFLINE;
            server = _transferDatabase.UpdateTransfer(server);
            log_info << "Transfer manager stopped, serverId: " << server.serverId;

            _transferDatabase.DeleteTransfer(request.serverId);
            log_info << "Transfer manager deleted, serverId: " << server.serverId;

        } catch (bsoncxx::exception &ex) {

            // Update state
            server.state = Database::Entity::Transfer::ServerState::STOP_FAILED;
            server = _transferDatabase.UpdateTransfer(server);

            log_error << "Start manager request failed, serverId: " << server.serverId << " message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void TransferService::DeleteUser(const Dto::Transfer::DeleteUserRequest &request) const {
        Monitoring::MetricServiceTimer measure(TRANSFER_SERVICE_TIMER, "method", "delete_user");

        Database::Entity::Transfer::Transfer server;
        try {
            if (!_transferDatabase.TransferExists(request.region, request.serverId)) {
                log_error << "Transfer server does not exist, serverId: " << request.serverId;
                throw Core::ServiceException("Transfer server does not exist, serverId: " + request.serverId);
            }

            // Get the manager
            server = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

            if (server.HasUser(request.userName)) {
                std::string userName = request.userName;
                std::erase_if(server.users,
                              [userName](const Database::Entity::Transfer::User &user) {
                                  return user.userName == userName;
                              });
            }

            // Update server
            server = _transferDatabase.UpdateTransfer(server);
            log_info << "User deleted, serverId: " << server.serverId << " userName: " << request.userName;

        } catch (bsoncxx::exception &ex) {
            log_error << "Delete user request failed, serverId: " << server.serverId << " message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }
}// namespace AwsMock::Service
