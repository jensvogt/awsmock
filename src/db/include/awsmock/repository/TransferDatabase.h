//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_TRANSFER_DATABASE_H
#define AWSMOCK_REPOSITORY_TRANSFER_DATABASE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/utils/SortColumn.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/transfer/Transfer.h>
#include <awsmock/memorydb/TransferMemoryDb.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * @brief Transfer manager MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TransferDatabase : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit TransferDatabase();

        /**
         * @brief Singleton instance
         */
        static TransferDatabase &instance() {
            static TransferDatabase transferDatabase;
            return transferDatabase;
        }

        /**
         * @brief Check the existence of a transfer server
         *
         * @param region AWS region name
         * @param serverId AWS server ID
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        bool TransferExists(const std::string &region, const std::string &serverId) const;

        /**
         * @brief Check the existence of a transfer server
         *
         * @param transfer AWS transfer
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        bool TransferExists(const Entity::Transfer::Transfer &transfer) const;

        /**
         * @brief Check the existence of the transfer server
         *
         * @param serverId AWS transfer server ID
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        bool TransferExists(const std::string &serverId) const;

        /**
         * @brief Check the existence of a transfer server
         *
         * @param region AWS region name
         * @param protocols list of protocols
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        bool TransferExists(const std::string &region, const std::vector<Entity::Transfer::Protocol> &protocols) const;

        /**
         * @brief Create a new transfer server
         *
         * @param transfer transfer entity
         * @return created transfer entity.
         */
        Entity::Transfer::Transfer CreateTransfer(const Entity::Transfer::Transfer &transfer) const;

        /**
         * @brief Updates an existing transfer manager
         *
         * @param transfer transfer entity
         * @return updated transfer entity.
         */
        Entity::Transfer::Transfer UpdateTransfer(const Entity::Transfer::Transfer &transfer) const;

        /**
         * @brief Created or updates an existing transfer manager
         *
         * @param transfer transfer entity
         * @return created or updated transfer entity.
         */
        Entity::Transfer::Transfer CreateOrUpdateTransfer(const Entity::Transfer::Transfer &transfer) const;

        /**
         * @brief Returns a transfer manager entity by primary key
         *
         * @param oid transfer manager primary key
         * @return transfer manager entity
         * @throws DatabaseException
         */
        Entity::Transfer::Transfer GetTransferById(bsoncxx::oid oid) const;

        /**
         * @brief Returns a transfer manager entity by primary key
         *
         * @param oid transfer manager primary key
         * @return transfer manager entity
         * @throws DatabaseException
         */
        Entity::Transfer::Transfer GetTransferById(const std::string &oid) const;

        /**
         * @brief Returns a transfer manager entity by manager ID
         *
         * @param region AWS region name
         * @param serverId transfer manager ID
         * @return transfer manager entity
         * @throws DatabaseException
         */
        Entity::Transfer::Transfer GetTransferByServerId(const std::string &region, const std::string &serverId) const;

        /**
         * @brief Returns a transfer manager entity by ARN
         *
         * @param arn transfer manager ARN
         * @return transfer manager entity
         * @throws DatabaseException
         */
        Entity::Transfer::Transfer GetTransferByArn(const std::string &arn) const;

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
        [[nodiscard]] std::vector<Entity::Transfer::Transfer> ListServers(const std::string &region = {}, const std::string &prefix = {}, long pageSize = 0, long pageIndex = 0, const std::vector<SortColumn> &sortColumns = {}) const;

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
        std::vector<Entity::Transfer::User> ListUsers(const std::string &region, const std::string &serverId, const std::string &prefix = {}, long pageSize = 0, long pageIndex = 0, const std::vector<SortColumn> &sortColumns = {}) const;

        /**
         * @brief Returns a list of transfer users.
         *
         * @param region AWS region name
         * @param maxResults maximal numbers of results
         * @param nextToken next token
         * @return list of transfer users
         */
        std::vector<Entity::Transfer::Transfer> ListServers(const std::string &region, std::string &nextToken, long maxResults = -1) const;

        /**
         * @brief Returns the total number of servers.
         *
         * @param region AWS region name
         * @return total number of transfer servers
         */
        long CountServers(const std::string &region = {}) const;

        /**
         * @brief Returns the total number of users for a server.
         *
         * @param region AWS region name
         * @param serverId server ID
         * @return total number of transfer server users
         */
        long CountUsers(const std::string &region = {}, const std::string &serverId = {}) const;

        /**
         * @brief Deletes an existing transfer manager
         *
         * @param serverId transfer serverID
         * @throws DatabaseException
         */
        void DeleteTransfer(const std::string &serverId) const;

        /**
         * @brief Deletes all existing transfer server
         *
         * @return total number of deleted objects
         * @throws DatabaseException
         */
        long DeleteAllTransfers() const;

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Server collection name
         */
        std::string _serverCollectionName;

        /**
         * Transfer in-memory database
         */
        TransferMemoryDb &_memoryDb;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_TRANSFER_DATABASE_H
