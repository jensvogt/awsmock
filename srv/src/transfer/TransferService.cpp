//
// Created by vogje01 on 30/05/2023.
//

#include "awsmock/dto/common/mapper/Mapper.h"
#include "awsmock/dto/dynamodb/mapper/Mapper.h"
#include "awsmock/dto/transfer/model/Tag.h"


#include <awsmock/service/transfer/TransferService.h>

namespace AwsMock::Service {

    Dto::Transfer::CreateServerResponse TransferService::CreateTransferServer(Dto::Transfer::CreateServerRequest &request) const {
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "create_transfer_server");
        log_debug << "Create transfer manager";

        // Check existence
        if (_transferDatabase.TransferExists(request.region, ProtocolTypeToString(request.protocols[0]))) {
            log_error << "Transfer manager exists already";
            throw Core::ServiceException("Transfer manager exists already");
        }

        std::string serverId = "s-" + Core::StringUtils::ToLower(Core::StringUtils::GenerateRandomHexString(20));

        Database::Entity::Transfer::Transfer transferEntity;
        auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        auto transferArn = Core::AwsUtils::CreateTransferArn(request.region, accountId, serverId);
        auto listenAddress = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.transfer.ftp.address");

        // Create entity
        int ftpPort = Core::Configuration::instance().GetValue<int>("awsmock.modules.transfer.ftp.port");
        transferEntity = {.region = request.region, .serverId = serverId, .arn = transferArn, .ports = {ftpPort}, .listenAddress = listenAddress};
        transferEntity.protocols.emplace_back(Database::Entity::Transfer::ProtocolFromString(ProtocolTypeToString(request.protocols[0])));

        // Add an anonymous user
        Database::Entity::Transfer::User anonymousUser = {.userName = "anonymous", .password = "secret", .homeDirectory = "/"};
        transferEntity.users.emplace_back(anonymousUser);

        transferEntity = _transferDatabase.CreateTransfer(transferEntity);

        // Create response
        Dto::Transfer::CreateServerResponse response;
        response.region = transferEntity.region;
        response.serverId = serverId;
        response.arn = transferArn;
        log_info << "Transfer manager created, address: " << listenAddress << " port: " << ftpPort;

        return response;
    }

    Dto::Transfer::CreateUserResponse TransferService::CreateUser(Dto::Transfer::CreateUserRequest &request) const {
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "create_user");
        log_debug << "Create user request";

        Database::Entity::Transfer::Transfer transferEntity;

        if (!_transferDatabase.TransferExists(request.region, request.serverId)) {
            log_error << "Transfer manager with ID '" + request.serverId + "  does not exist";
            throw Core::ServiceException("Transfer manager with ID '" + request.serverId + "  does not exist");
        }
        transferEntity = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

        // Check user
        if (transferEntity.HasUser(request.userName)) {
            log_warning << "Transfer manager has already a user with name '" + request.userName + "'";
            Database::Entity::Transfer::User user = transferEntity.GetUser(request.userName);
            Dto::Transfer::CreateUserResponse response;
            response.region = transferEntity.region;
            response.serverId = transferEntity.serverId;
            response.userName = user.userName;
            return response;
        }

        // Get home directory
        std::string homeDirectory = request.userName;
        if (!Core::StringUtils::IsNullOrEmpty(&request.homeDirectory)) {
            homeDirectory = request.homeDirectory;
        }

        // Add user
        auto accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        std::string userArn = Core::AwsUtils::CreateTransferUserArn(request.region, accountId, transferEntity.serverId, request.userName);
        Database::Entity::Transfer::User user = {
                .userName = request.userName,
                .password = Core::StringUtils::GenerateRandomPassword(8),
                .homeDirectory = homeDirectory,
                .arn = userArn};

        // Set the password when provided by request
        if (!request.password.empty()) {
            user.password = request.password;
        }
        transferEntity.users.emplace_back(user);

        // Update database
        transferEntity = _transferDatabase.UpdateTransfer(transferEntity);
        log_debug << "Updated transfer manager, serverId: " << transferEntity.serverId;

        // Create user in transfer server

        // Create response
        Dto::Transfer::CreateUserResponse response;
        response.region = transferEntity.region;
        response.serverId = transferEntity.serverId;
        response.userName = request.userName;

        return response;
    }

    void TransferService::CreateProtocol(Dto::Transfer::CreateProtocolRequest &request) const {
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "create_protocol");
        log_debug << "Create protocol request";

        if (!_transferDatabase.TransferExists(request.region, request.serverId)) {
            log_error << "Transfer manager with ID '" + request.serverId + "  does not exist";
            throw Core::ServiceException("Transfer manager with ID '" + request.serverId + "  does not exist");
        }
        Database::Entity::Transfer::Transfer transferEntity = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

        // Check protocol
        if (transferEntity.HasProtocol(ProtocolTypeToString(request.protocol))) {
            log_warning << "Transfer manager has already a protocol with name '" + ProtocolTypeToString(request.protocol) + "'";
            return;
        }

        // Add protocol
        transferEntity.protocols.emplace_back(Database::Entity::Transfer::ProtocolFromString(ProtocolTypeToString(request.protocol)));
        transferEntity.ports.emplace_back(request.port);

        // Update database
        transferEntity = _transferDatabase.UpdateTransfer(transferEntity);
        log_debug << "Updated transfer manager, serverId: " << transferEntity.serverId;
    }

    Dto::Transfer::ListServerResponse TransferService::ListServers(const Dto::Transfer::ListServerRequest &request) const {
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "list_servers");

        try {
            std::string nextToken = request.nextToken;
            const std::vector<Database::Entity::Transfer::Transfer> servers = _transferDatabase.ListServers(request.region, nextToken, request.maxResults);

            auto response = Dto::Transfer::ListServerResponse();
            response.nextToken = nextToken;
            for (const auto &s: servers) {
                Dto::Transfer::Server server;
                server.arn = s.arn;
                server.serverId = s.serverId;
                server.state = ServerStateToString(s.state);
                server.userCount = static_cast<int>(s.users.size());
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
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "list_server_counters");

        try {
            const std::vector<Database::SortColumn> sortColumns = Dto::Common::Mapper::map(request.sortColumns);
            const std::vector<Database::Entity::Transfer::Transfer> servers = _transferDatabase.ListServers(request.region, request.prefix, request.pageSize, request.pageIndex, sortColumns);

            auto response = Dto::Transfer::ListServerCountersResponse();
            response.total = _transferDatabase.CountServers(request.region);
            for (const auto &s: servers) {
                Dto::Transfer::Server server;
                server.region = request.region;
                server.arn = s.arn;
                server.serverId = s.serverId;
                server.state = ServerStateToString(s.state);
                server.userCount = static_cast<int>(s.users.size());
                server.lastStarted = s.lastStarted;
                server.created = s.created;
                server.modified = s.modified;
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
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "get_server_details");

        // Check existence
        if (!_transferDatabase.TransferExists(request.region, request.serverId)) {
            log_error << "Transfer server does not exist, region: " << request.region << " serverId: " << request.serverId;
            throw Core::ServiceException("Transfer server does not exist, region: " + request.region + " serverId: " + request.serverId);
        }

        try {
            Database::Entity::Transfer::Transfer server = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

            auto response = Dto::Transfer::GetServerDetailsResponse();
            response.server.region = server.region;
            response.server.arn = server.arn;
            response.server.serverId = server.serverId;
            response.server.state = ServerStateToString(server.state);
            response.server.userCount = static_cast<int>(server.users.size());
            response.server.ports = server.ports;
            response.server.concurrency = server.concurrency;
            response.server.lastStarted = server.lastStarted;
            response.server.created = server.created;
            response.server.modified = server.modified;

            log_trace << "Transfer server list outcome: " + response.ToJson();
            return response;
        } catch (bsoncxx::exception &ex) {
            log_error << "Transfer server list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Transfer::ListUsersResponse TransferService::ListUsers(const Dto::Transfer::ListUsersRequest &request) const {
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "list_users");

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
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "list_user_counters");

        try {
            std::vector<Database::Entity::Transfer::User> users = _transferDatabase.ListUsers(request.region, request.serverId, request.prefix, request.pageSize, request.pageIndex, Dto::Common::Mapper::map(request.sortColumns));

            auto response = Dto::Transfer::ListUserCountersResponse();
            response.total = _transferDatabase.CountUsers(request.region, request.serverId);
            for (const auto &user: users) {
                Dto::Transfer::User userDto;
                userDto.region = request.region;
                userDto.userName = user.userName;
                userDto.arn = user.arn;
                userDto.password = user.password;
                response.userCounters.emplace_back(userDto);
            }

            log_trace << "Transfer user list result: " << response.ToJson();
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "Transfer user list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Transfer::ListTagCountersResponse TransferService::ListTagCounters(const Dto::Transfer::ListTagCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "list_tag_counters");

        try {
            std::vector<Database::SortColumn> sortColumns;
            for (const auto &s: request.sortColumns) {
                Database::SortColumn sortColumn;
                sortColumn.column = request.sortColumns[0].column;
                sortColumn.sortDirection = request.sortColumns[0].sortDirection;
                sortColumns.emplace_back(sortColumn);
            }

            Database::Entity::Transfer::Transfer server = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

            Dto::Transfer::ListTagCountersResponse response;
            response.total = static_cast<long>(server.tags.size());

            int index = 0;
            for (const auto &[fst, snd]: server.tags) {
                if (index >= request.pageIndex * request.pageSize && index < (request.pageIndex + 1) * request.pageSize) {
                    response.tagCounters[fst] = snd;
                }
                index++;
            }

            log_trace << "Transfer tag list result: " << response.ToJson();
            return response;

        } catch (bsoncxx::exception &ex) {
            log_error << "Transfer tag list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    Dto::Transfer::ListProtocolCountersResponse TransferService::ListProtocolCounters(const Dto::Transfer::ListProtocolCountersRequest &request) const {
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "list_protocol_counters");

        try {
            const Database::Entity::Transfer::Transfer server = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

            Dto::Transfer::ListProtocolCountersResponse response;
            response.total = static_cast<long>(server.protocols.size());
            for (int i = 0; i < server.protocols.size(); i++) {
                Dto::Transfer::ProtocolCounter protocolDto;
                protocolDto.protocol = Dto::Transfer::ProtocolTypeFromString(ProtocolToString(server.protocols.at(i)));
                protocolDto.port = server.ports.at(i);
                response.protocolCounters.emplace_back(protocolDto);
            }

            log_trace << "Transfer protocol list result: " << response.ToJson();
            return response;
        } catch (bsoncxx::exception &ex) {
            log_error << "Transfer protocol list request failed, message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }

    void TransferService::StartServer(const Dto::Transfer::StartServerRequest &request) const {
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "start_server");

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
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "stop_server");

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
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "delete_server");

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
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "delete_user");

        if (!_transferDatabase.TransferExists(request.region, request.serverId)) {
            log_error << "Transfer server does not exist, serverId: " << request.serverId;
            throw Core::ServiceException("Transfer server does not exist, serverId: " + request.serverId);
        }

        Database::Entity::Transfer::Transfer server;
        try {

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

    void TransferService::DeleteProtocol(const Dto::Transfer::DeleteProtocolRequest &request) const {
        Monitoring::MonitoringTimer measure(TRANSFER_SERVICE_TIMER, "method", "delete_protocol");

        if (!_transferDatabase.TransferExists(request.region, request.serverId)) {
            log_error << "Transfer server does not exist, serverId: " << request.serverId;
            throw Core::ServiceException("Transfer server does not exist, serverId: " + request.serverId);
        }

        Database::Entity::Transfer::Transfer server;
        try {

            // Get the manager
            server = _transferDatabase.GetTransferByServerId(request.region, request.serverId);

            if (server.HasProtocol(ProtocolTypeToString(request.protocol))) {
                std::string protocolName = ProtocolTypeToString(request.protocol);
                std::erase_if(server.protocols,
                              [protocolName](const Database::Entity::Transfer::Protocol &protocol) {
                                  return protocol == Database::Entity::Transfer::ProtocolFromString(protocolName);
                              });
            }

            // Update server
            server = _transferDatabase.UpdateTransfer(server);
            log_info << "Protocol deleted, serverId: " << server.serverId << " protocolName: " << ProtocolTypeToString(request.protocol);

        } catch (bsoncxx::exception &ex) {
            log_error << "Delete protocol request failed, serverId: " << server.serverId << " message: " << ex.what();
            throw Core::ServiceException(ex.what());
        }
    }
}// namespace AwsMock::Service
