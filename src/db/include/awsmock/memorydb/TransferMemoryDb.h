//
// Created by vogje01 on 11/19/23.
//

#ifndef AWSMOCK_REPOSITORY_TRANSFER_MEMORYDB_H
#define AWSMOCK_REPOSITORY_TRANSFER_MEMORYDB_H

// C++ includes
#include <string>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/transfer/Transfer.h>
#include <awsmock/entity/transfer/User.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    /**
     * @brief Transfer manager in-memory database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class TransferMemoryDb {

      public:

        /**
         * @brief Constructor
         */
        TransferMemoryDb() = default;

        /**
         * @brief Singleton instance
         */
        static TransferMemoryDb &instance() {
            static TransferMemoryDb transferMemoryDb;
            return transferMemoryDb;
        }

        /**
         * @brief Check existence of transfer servers
         *
         * @param region AWS region name
         * @param serverId AWS server ID
         * @return true if transfer manager already exists
         * @throws DatabaseException
         */
        bool TransferExists(const std::string &region, const std::string &serverId);

        /**
         * @brief Check existence of transfer servers
         *
         * @param transfer AWS transfer
         * @return true if transfer manager already exists
         * @throws DatabaseException
         */
        bool TransferExists(const Entity::Transfer::Transfer &transfer);

        /**
         * @brief Check existence of transfer servers
         *
         * @param serverId AWS server ID
         * @return true if transfer manager already exists
         * @throws DatabaseException
         */
        bool TransferExists(const std::string &serverId);

        /**
         * @brief Check existence of transfer servers by protocol
         *
         * @param region AWS region name
         * @param protocols list of protocols
         * @return true if transfer manager already exists
         * @throws DatabaseException
         */
        bool TransferExists(const std::string &region, const std::vector<Entity::Transfer::Protocol> &protocols);

        /**
         * @brief Returns a list of transfer manager.
         *
         * @param region AWS region name
         * @return list of transfer manager
         */
        std::vector<Entity::Transfer::Transfer> ListServers(const std::string &region);

        /**
         * @brief Returns a list of transfer users.
         *
         * @param region AWS region name
         * @param serverId AWS server ID
         * @return list of transfer users
         */
        std::vector<Entity::Transfer::User> ListUsers(const std::string &region, const std::string &serverId);

        /**
         * @brief Create a new transfer servers
         *
         * @param transfer transfer entity
         * @return created transfer entity.
         */
        Entity::Transfer::Transfer CreateTransfer(const Entity::Transfer::Transfer &transfer);

        /**
         * @brief Updates an existing transfer manager
         *
         * @param transfer transfer entity
         * @return updated transfer entity.
         */
        Entity::Transfer::Transfer UpdateTransfer(const Entity::Transfer::Transfer &transfer);

        /**
         * @brief Returns a transfer manager entity by primary key
         *
         * @param oid transfer manager primary key
         * @return transfer manager entity
         * @throws DatabaseException
         */
        Entity::Transfer::Transfer GetTransferById(const std::string &oid);

        /**
         * @brief Returns a transfer manager entity by manager ID
         *
         * @param region AWS region name
         * @param serverId transfer manager ID
         * @return transfer manager entity
         * @throws DatabaseException
         */
        Entity::Transfer::Transfer GetTransferByServerId(const std::string &region, const std::string &serverId);

        /**
         * @brief Returns a transfer manager entity by ARN
         *
         * @param arn transfer manager ARN
         * @return transfer manager entity
         * @throws DatabaseException
         */
        Entity::Transfer::Transfer GetTransferByArn(const std::string &arn);

        /**
         * @brief Returns the total number of servers.
         *
         * @param region AWS region name
         * @return total number of transfer server
         */
        long CountServers(const std::string &region = {});

        /**
         * @brief Deletes an existing transfer manager
         *
         * @param serverId transfer server ID
         * @throws DatabaseException
         */
        void DeleteTransfer(const std::string &serverId);

        /**
         * @brief Deletes all existing transfer server
         *
         * @return total number of deleted objects
         * @throws DatabaseException
         */
        long DeleteAllTransfers();

      private:

        /**
         * Transfer server map, when running without database
         */
        std::map<std::string, Entity::Transfer::Transfer> _transfers;

        /**
         * Transfer user map, when running without database
         */
        std::map<std::string, Entity::Transfer::User> _users;

        /**
         * Transfer mutex
         */
        static boost::mutex _transferMutex;

        /**
         * User mutex
         */
        static boost::mutex _userMutex;
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_TRANSFER_MEMORYDB_H
