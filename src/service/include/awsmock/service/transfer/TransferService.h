//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_SERVICE_TRANSFER_SERVICE_H
#define AWSMOCK_SERVICE_TRANSFER_SERVICE_H

// AwsMock includes
#include <awsmock/core/monitoring/MonitoringDefinition.h>
#include <awsmock/core/monitoring/MonitoringTimer.h>
#include <awsmock/dto/transfer/CreateProtocolRequest.h>
#include <awsmock/dto/transfer/CreateServerRequest.h>
#include <awsmock/dto/transfer/CreateServerResponse.h>
#include <awsmock/dto/transfer/CreateUserRequest.h>
#include <awsmock/dto/transfer/CreateUserResponse.h>
#include <awsmock/dto/transfer/DeleteProtocolRequest.h>
#include <awsmock/dto/transfer/DeleteServerRequest.h>
#include <awsmock/dto/transfer/DeleteUserRequest.h>
#include <awsmock/dto/transfer/GetServerDetailsRequest.h>
#include <awsmock/dto/transfer/GetServerDetailsResponse.h>
#include <awsmock/dto/transfer/ListServerRequest.h>
#include <awsmock/dto/transfer/ListServerResponse.h>
#include <awsmock/dto/transfer/ListUsersRequest.h>
#include <awsmock/dto/transfer/ListUsersResponse.h>
#include <awsmock/dto/transfer/StartServerRequest.h>
#include <awsmock/dto/transfer/StopServerRequest.h>
#include <awsmock/dto/transfer/internal/ListProtocolCountersRequest.h>
#include <awsmock/dto/transfer/internal/ListProtocolCountersResponse.h>
#include <awsmock/dto/transfer/internal/ListServerCountersRequest.h>
#include <awsmock/dto/transfer/internal/ListServerCountersResponse.h>
#include <awsmock/dto/transfer/internal/ListTagCountersRequest.h>
#include <awsmock/dto/transfer/internal/ListTagCountersResponse.h>
#include <awsmock/dto/transfer/internal/ListUserCountersRequest.h>
#include <awsmock/dto/transfer/internal/ListUserCountersResponse.h>
#include <awsmock/dto/transfer/mapper/Mapper.h>
#include <awsmock/repository/TransferDatabase.h>

#define TRANSFER_DEFAULT_FTP_PORT 21

namespace AwsMock::Service {

    /**
     * @brief Transfer family service
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TransferService {

      public:

        /**
         * @brief Constructor
         */
        explicit TransferService() = default;

        /**
         * @brief Create transfer server request
         *
         * @param request create server request
         * @return CreateServerResponse
         * @see Dto::Transfer::CreateServerResponse
         */
        Dto::Transfer::CreateServerResponse CreateTransferServer(Dto::Transfer::CreateServerRequest &request) const;

        /**
         * @brief Create a user for the transfer manager.
         *
         * @param request create user request
         * @return CreateUserResponse
         * @see Dto::Transfer::CreateUserResponse
         */
        Dto::Transfer::CreateUserResponse CreateUser(Dto::Transfer::CreateUserRequest &request) const;

        /**
         * @brief Create a protocol for the transfer manager.
         *
         * @param request create user request
         * @see Dto::Transfer::CreateUserResponse
         */
        void CreateProtocol(Dto::Transfer::CreateProtocolRequest &request) const;

        /**
         * @brief Returns a list of available servers
         *
         * @param request list manager request
         * @return ListServerResponse
         * @see Dto::Transfer::ListServerResponse
         */
        Dto::Transfer::ListServerResponse ListServers(const Dto::Transfer::ListServerRequest &request) const;

        /**
         * @brief Returns a list of available server counters
         *
         * @param request list server counters request
         * @return ListServerCountersResponse
         * @see Dto::Transfer::ListServerCountersResponse
         */
        Dto::Transfer::ListServerCountersResponse ListServerCounters(const Dto::Transfer::ListServerCountersRequest &request) const;

        /**
         * @brief Returns a list of available servers
         *
         * @param request list manager request
         * @return ListServerResponse
         * @see Dto::Transfer::ListServerResponse
         */
        Dto::Transfer::ListUsersResponse ListUsers(const Dto::Transfer::ListUsersRequest &request) const;

        /**
         * @brief Returns a list of available user counters
         *
         * @param request list user counters request
         * @return ListUserCountersResponse
         * @see Dto::Transfer::ListUserCountersResponse
         */
        Dto::Transfer::ListUserCountersResponse ListUserCounters(const Dto::Transfer::ListUserCountersRequest &request) const;

        /**
         * @brief Returns a list of available tag counters
         *
         * @param request list tag counters request
         * @return ListTagCountersResponse
         * @see Dto::Transfer::ListTagCountersResponse
         */
        Dto::Transfer::ListTagCountersResponse ListTagCounters(const Dto::Transfer::ListTagCountersRequest &request) const;

        /**
         * @brief Returns a list of available protocol counters for a server
         *
         * @param request list protocol counters request
         * @return ListProtocolCountersResponse
         * @see Dto::Transfer::ListProtocolCountersResponse
         */
        Dto::Transfer::ListProtocolCountersResponse ListProtocolCounters(const Dto::Transfer::ListProtocolCountersRequest &request) const;

        /**
         * @brief Starts a manager.
         *
         * @param request StartServer manager request
         */
        void StartServer(const Dto::Transfer::StartServerRequest &request) const;

        /**
         * @brief Stops a manager.
         *
         * @param request stop manager request
         */
        void StopServer(const Dto::Transfer::StopServerRequest &request) const;

        /**
         * @brief Returns the transfer server details.
         *
         * @param request delete manager request
         * @return GetServerDetailsResponse DTO
         * @see GetServerDetailsResponse
         */
        Dto::Transfer::GetServerDetailsResponse GetServerDetails(const Dto::Transfer::GetServerDetailsRequest &request) const;

        /**
         * @brief Deleted a manager.
         *
         * @param request delete manager request
         */
        void DeleteServer(const Dto::Transfer::DeleteServerRequest &request) const;

        /**
         * @brief Deleted a user from a server.
         *
         * @param request delete user request
         */
        void DeleteUser(const Dto::Transfer::DeleteUserRequest &request) const;

        /**
         * @brief Deleted a protocol from a server.
         *
         * @param request delete protocol request
         */
        void DeleteProtocol(const Dto::Transfer::DeleteProtocolRequest &request) const;

      private:

        /**
         * Transfer database connection
         */
        Database::TransferDatabase &_transferDatabase = Database::TransferDatabase::instance();
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_TRANSFER_SERVICE_H
