//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/transfer/Transfer.h>
#include <awsmock/repository/transfer/ITransferRepository.h>
#include <awsmock/utils/ConnectionPool.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Transfer manager MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TransferMongoRepository final : public ITransferRepository {

      public:

        /**
         * @brief Constructor
         */
        explicit TransferMongoRepository() = default;

        /**
         * @brief Check the existence of a transfer server
         *
         * @param region AWS region name
         * @param serverId AWS server ID
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        bool transferExists(const std::string &region, const std::string &serverId) const override;

        /**
         * @brief Check the existence of a transfer server
         *
         * @param transfer AWS transfer
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        bool transferExists(const Entity::Transfer::Transfer &transfer) const override;

        /**
         * @brief Check the existence of the transfer server
         *
         * @param serverId AWS transfer server ID
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        bool transferExists(const std::string &serverId) const override;

        /**
         * @brief Check the existence of a transfer server
         *
         * @param region AWS region name
         * @param protocols list of protocols
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        bool transferExists(const std::string &region, const std::vector<Entity::Transfer::Protocol> &protocols) const override;

        /**
         * @brief Create a new transfer server
         *
         * @param transfer transfer entity
         * @return created transfer entity.
         */
        Entity::Transfer::Transfer createTransfer(const Entity::Transfer::Transfer &transfer) const override;

        /**
         * @brief Updates an existing transfer manager
         *
         * @param transfer transfer entity
         * @return updated transfer entity.
         */
        Entity::Transfer::Transfer updateTransfer(const Entity::Transfer::Transfer &transfer) const override;

        /**
         * @brief Created or updates an existing transfer manager
         *
         * @param transfer transfer entity
         * @return created or updated transfer entity.
         */
        Entity::Transfer::Transfer createOrUpdateTransfer(const Entity::Transfer::Transfer &transfer) const override;

        /**
         * @brief Returns a transfer manager entity by primary key
         *
         * @param oid transfer manager primary key
         * @return transfer manager entity
         * @throws DatabaseException
         */
        Entity::Transfer::Transfer getTransferById(const bsoncxx::oid &oid) const override;

        /**
         * @brief Returns a transfer manager entity by primary key
         *
         * @param oid transfer manager primary key
         * @return transfer manager entity
         * @throws DatabaseException
         */
        Entity::Transfer::Transfer getTransferById(const std::string &oid) const override;

        /**
         * @brief Returns a transfer manager entity by manager ID
         *
         * @param region AWS region name
         * @param serverId transfer manager ID
         * @return transfer manager entity
         * @throws DatabaseException
         */
        Entity::Transfer::Transfer getTransferByServerId(const std::string &region, const std::string &serverId) const override;

        /**
         * @brief Returns a transfer manager entity by ARN
         *
         * @param arn transfer manager ARN
         * @return transfer manager entity
         * @throws DatabaseException
         */
        Entity::Transfer::Transfer getTransferByArn(const std::string &arn) const override;

        /**
         * @brief Returns a list of transfer manager.
         *
         * @param region AWS region name
         * @param prefix server ID prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sorting
         * @return list of transfer manager
         */
        [[nodiscard]] std::vector<Entity::Transfer::Transfer> listServers(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Returns a list of transfer users.
         *
         * @param region AWS region name
         * @param serverId AWS server ID
         * @param prefix user name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sorting column names
         * @return list of transfer users
         */
        std::vector<Entity::Transfer::User> listUsers(const std::string &region, const std::string &serverId, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Returns a list of transfer users.
         *
         * @param region AWS region name
         * @param maxResults maximal numbers of results
         * @param nextToken next token
         * @return list of transfer users
         */
        std::vector<Entity::Transfer::Transfer> listServers(const std::string &region, std::string &nextToken, long maxResults) const override;

        /**
         * @brief Returns the total number of servers.
         *
         * @param region AWS region name
         * @return total number of transfer servers
         */
        long countServers(const std::string &region) const override;

        /**
         * @brief Returns the total number of users for a server.
         *
         * @param region AWS region name
         * @param serverId server ID
         * @return total number of transfer server users
         */
        long countUsers(const std::string &region = {}, const std::string &serverId = {}) const override;

        /**
         * @brief Deletes an existing transfer manager
         *
         * @param serverId transfer serverID
         * @throws DatabaseException
         */
        void deleteTransfer(const std::string &serverId) const override;

        /**
         * @brief Deletes all existing transfer server
         *
         * @return total number of deleted objects
         * @throws DatabaseException
         */
        long deleteAllTransfers() const override;

      private:

        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Transfer"};

        /**
         * Database name
         */
        static constexpr std::string _databaseName = "awsmock";

        /**
         * Server collection name
         */
        static constexpr std::string _serverCollectionName = "transfer";
    };

}// namespace Awsmock::Database
