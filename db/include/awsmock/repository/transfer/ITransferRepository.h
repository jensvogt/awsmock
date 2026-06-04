//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/transfer/Transfer.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for transfer repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * transfer-related data.
     */
    class ITransferRepository {

      public:

        /**
         * @brief Virtual destructor for the ITransferRepository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~ITransferRepository() = default;

        /**
         * @brief Check the existence of a transfer server
         *
         * @param region AWS region name
         * @param serverId AWS server ID
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool transferExists(const std::string &region, const std::string &serverId) const = 0;

        /**
         * @brief Check the existence of a transfer server
         *
         * @param transfer AWS transfer
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool transferExists(const Entity::Transfer::Transfer &transfer) const = 0;

        /**
         * @brief Check the existence of the transfer server
         *
         * @param serverId AWS transfer server ID
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool transferExists(const std::string &serverId) const = 0;

        /**
         * @brief Check the existence of a transfer server
         *
         * @param region AWS region name
         * @param protocols list of protocols
         * @return true if the transfer server already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool transferExists(const std::string &region, const std::vector<Entity::Transfer::Protocol> &protocols) const = 0;

        /**
         * @brief Create a new transfer server
         *
         * @param transfer transfer entity
         * @return the created transfer entity.
         */
        [[nodiscard]]
        virtual Entity::Transfer::Transfer createTransfer(const Entity::Transfer::Transfer &transfer) const = 0;

        /**
         * @brief Updates an existing transfer manager
         *
         * @param transfer transfer entity
         * @return updated transfer entity.
         */
        [[nodiscard]]
        virtual Entity::Transfer::Transfer updateTransfer(const Entity::Transfer::Transfer &transfer) const = 0;

        /**
         * @brief Created or updates an existing transfer manager
         *
         * @param transfer transfer entity
         * @return created or updated transfer entity.
         */
        [[nodiscard]]
        virtual Entity::Transfer::Transfer createOrUpdateTransfer(const Entity::Transfer::Transfer &transfer) const = 0;

        /**
         * @brief Returns a transfer manager entity by primary key
         *
         * @param oid transfer manager primary key
         * @return transfer manager entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Transfer::Transfer getTransferById(const bsoncxx::oid &oid) const = 0;

        /**
         * @brief Returns a transfer manager entity by primary key
         *
         * @param oid transfer manager primary key
         * @return transfer manager entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Transfer::Transfer getTransferById(const std::string &oid) const = 0;

        /**
         * @brief Returns a transfer manager entity by manager ID
         *
         * @param region AWS region name
         * @param serverId transfer manager ID
         * @return transfer manager entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Transfer::Transfer getTransferByServerId(const std::string &region, const std::string &serverId) const = 0;

        /**
         * @brief Returns a transfer manager entity by ARN
         *
         * @param arn transfer manager ARN
         * @return transfer manager entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Transfer::Transfer getTransferByArn(const std::string &arn) const = 0;

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
        [[nodiscard]]
        virtual std::vector<Entity::Transfer::Transfer> listServers(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

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
        [[nodiscard]]
        virtual std::vector<Entity::Transfer::User> listUsers(const std::string &region, const std::string &serverId, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Returns a list of transfer users.
         *
         * @param region AWS region name
         * @param maxResults maximal numbers of results
         * @param nextToken next token
         * @return list of transfer users
         */
        [[nodiscard]]
        virtual std::vector<Entity::Transfer::Transfer> listServers(const std::string &region, std::string &nextToken, long maxResults) const = 0;

        /**
         * @brief Returns the total number of servers.
         *
         * @param region AWS region name
         * @return total number of transfer servers
         */
        [[nodiscard]]
        virtual long countServers(const std::string &region) const = 0;

        /**
         * @brief Returns the total number of users for a server.
         *
         * @param region AWS region name
         * @param serverId server ID
         * @return total number of transfer server users
         */
        [[nodiscard]]
        virtual long countUsers(const std::string &region, const std::string &serverId) const = 0;

        /**
         * @brief Deletes an existing transfer manager
         *
         * @param serverId transfer serverID
         * @throws DatabaseException
         */
        virtual void deleteTransfer(const std::string &serverId) const = 0;

        /**
         * @brief Deletes all existing transfer server
         *
         * @return total number of deleted objects
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllTransfers() const = 0;
    };

}// namespace Awsmock::Database