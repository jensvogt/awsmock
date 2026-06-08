//
// Created by vogje01 on 11/19/23.
//

#pragma once

// C++ includes
#include <ranges>
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/transfer/Transfer.h>
#include <awsmock/entity/transfer/User.h>
#include <awsmock/repository/transfer/ITransferRepository.h>

namespace Awsmock::Database {

    /**
     * @brief Transfer manager in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TransferMemoryRepository final : public ITransferRepository {

    public:
        /**
         * @brief Constructor
         */
        TransferMemoryRepository() = default;

        /**
         * @brief Check the existence of transfer servers
         *
         * @param region AWS region name
         * @param serverId AWS server ID
         * @return true if a transfer manager already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool transferExists(const std::string &region, const std::string &serverId) const override;

        /**
         * @brief Check the existence of transfer servers
         *
         * @param transfer AWS transfer
         * @return true if a transfer manager already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool transferExists(const Entity::Transfer::Transfer &transfer) const override;

        /**
         * @brief Check the existence of transfer servers
         *
         * @param serverId AWS server ID
         * @return true if a transfer manager already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool transferExists(const std::string &serverId) const override;

        /**
         * @brief Check the existence of transfer servers by protocol
         *
         * @param region AWS region name
         * @param protocols list of protocols
         * @return true if a transfer manager already exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool transferExists(const std::string &region, const std::vector<Entity::Transfer::Protocol> &protocols) const override;

        /**
         * @brief Returns a list of transfer manager.
         *
         * @param region AWS region name
         * @param prefix name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sorting columns
         * @return list of transfer manager
         */
        [[nodiscard]]
        std::vector<Entity::Transfer::Transfer> listServers(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Returns a list of transfer manager.
         *
         * @param region AWS region name
         * @param nextToken next token
         * @param maxResults maximal number of results
         * @return list of transfer manager
         */
        [[nodiscard]]
        std::vector<Entity::Transfer::Transfer> listServers(const std::string &region, std::string &nextToken, long maxResults) const override;

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
        std::vector<Entity::Transfer::User> listUsers(const std::string &region, const std::string &serverId, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Create a new transfer server
         *
         * @param transfer transfer entity
         * @return the created transfer entity.
         */
        [[nodiscard]]
        Entity::Transfer::Transfer createTransfer(const Entity::Transfer::Transfer &transfer) const override;

        /**
         * @brief Create a new transfer server or updates an existing one
         *
         * @param transfer transfer entity
         * @return the created transfer entity.
         */
        [[nodiscard]]
        Entity::Transfer::Transfer createOrUpdateTransfer(const Entity::Transfer::Transfer &transfer) const override;

        /**
         * @brief Updates an existing transfer manager
         *
         * @param transfer transfer entity
         * @return updated transfer entity.
         */
        [[nodiscard]]
        Entity::Transfer::Transfer updateTransfer(const Entity::Transfer::Transfer &transfer) const override;

        /**
         * @brief Returns a transfer manager entity by primary key
         *
         * @param oid transfer manager primary key
         * @return transfer manager entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Transfer::Transfer getTransferById(const std::string &oid) const override;

        /**
         * @brief Returns a transfer manager entity by primary key
         *
         * @param oid transfer manager primary key
         * @return transfer manager entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Transfer::Transfer getTransferById(const bsoncxx::oid &oid) const override;

        /**
         * @brief Returns a transfer manager entity by manager ID
         *
         * @param region AWS region name
         * @param serverId transfer manager ID
         * @return transfer manager entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Transfer::Transfer getTransferByServerId(const std::string &region, const std::string &serverId) const override;

        /**
         * @brief Returns a transfer manager entity by ARN
         *
         * @param arn transfer manager ARN
         * @return transfer manager entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Transfer::Transfer getTransferByArn(const std::string &arn) const override;

        /**
         * @brief Returns the total number of servers.
         *
         * @param region AWS region name
         * @return total number of transfer servers
         */
        [[nodiscard]]
        long countServers(const std::string &region) const override;

        /**
         * @brief Returns the total number of servers.
         *
         * @param region AWS region name
         * @param serverId server ID
         * @return total number of transfer servers
         */
        [[nodiscard]]
        long countUsers(const std::string &region, const std::string &serverId) const override;

        /**
         * @brief Deletes an existing transfer manager
         *
         * @param serverId transfer server ID
         * @throws DatabaseException
         */
        void deleteTransfer(const std::string &serverId) const override;

        /**
         * @brief Deletes all existing transfer server
         *
         * @return total number of deleted objects
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteAllTransfers() const override;

    private:
        mutable logger_t _logger{boost::log::keywords::channel = "Transfer"};

        /**
         * Transfer server map when running without a database
         */
        mutable std::map<std::string, Entity::Transfer::Transfer> _transfers;

        /**
         * Transfer user map when running without a database
         */
        mutable std::map<std::string, Entity::Transfer::User> _users;

        /**
         * Transfer mutex
         */
        static boost::mutex _transferMutex;

        /**
         * User mutex
         */
        static boost::mutex _userMutex;
    };

} // namespace Awsmock::Database
