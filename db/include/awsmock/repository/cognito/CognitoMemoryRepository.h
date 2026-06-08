//
// Created by vogje01 on 29/05/2023.
//

#pragma once

// C++ standard includes
#include <string>
#include <vector>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/entity/cognito/Group.h>
#include <awsmock/entity/cognito/User.h>
#include <awsmock/entity/cognito/UserPool.h>
#include <awsmock/repository/cognito/ICognitoRepository.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Cognito in-memory database.
     *
     * @par
     * Provides an in-memory database using a simple hash map. The key is a randomly generated UUID.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class CognitoMemoryRepository final : public ICognitoRepository {

    public:
        /**
         * @brief Constructor
         */
        CognitoMemoryRepository() = default;

        /**
         * @brief Check the existence of the cognito user pool
         *
         * @param region AWS region name
         * @param name cognito user pool name
         * @return true if cognito already exists
         * @throws DatabaseException
         */
        bool userPoolExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Check the existence of the cognito user pool
         *
         * @param userPoolId user pool userPoolId
         * @return true if cognito user pool exists
         * @throws DatabaseException
         */
        bool userPoolExists(const std::string &userPoolId) const override;

        /**
         * @brief Create a new cognito user pool
         *
         * @param userPool cognito user pool entity to create
         * @return created cognito user pool entity.
         */
        Entity::Cognito::UserPool createUserPool(Entity::Cognito::UserPool &userPool) const override;

        /**
         * @brief Create a new cognito user pool
         *
         * @param userPool cognito user pool entity to create
         * @return created cognito user pool entity.
         */
        Entity::Cognito::UserPool createOrUpdateUserPool(Entity::Cognito::UserPool &userPool) const override;

        /**
         * @brief Returns a cognito user pool entity by primary key
         *
         * @param userPoolId cognito user pool ID
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        Entity::Cognito::UserPool getUserPoolByUserPoolId(const std::string &userPoolId) const override;

        /**
         * @brief Returns a cognito user pool entity by client Id
         *
         * @param clientId cognito client ID
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        Entity::Cognito::UserPool getUserPoolByClientId(const std::string &clientId) const override;

        /**
         * @brief Returns a cognito user pool entity by ID
         *
         * @param oid ID of the user pool
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        Entity::Cognito::UserPool getUserPoolById(bsoncxx::oid oid) const override;

        /**
         * @brief Returns a cognito user pool entity by region and name
         *
         * @param region AWS region
         * @param name cognito user pool name
         * @return cognito entity
         * @throws DatabaseException
         */
        Entity::Cognito::UserPool getUserPoolByRegionName(const std::string &region, const std::string &name) const override;

        /**
         * @brief Count all user pools
         *
         * @param region aws-mock region.
         * @return total number of user pools.
         */
        long countUserPools(const std::string &region) const override;

        /**
         * @brief Returns a list of cognito user pools.
         *
         * @param region AWS region name
         * @param prefix name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sort columns
         * @return list of cognito user pools
         */
        std::vector<Entity::Cognito::UserPool> listUserPools(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Exports a list of cognito user pools.
         *
         * @param sortColumns sort columns
         * @return list of cognito user pools
         */
        std::vector<Entity::Cognito::UserPool> exportUserPools(const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Updates an existing cognito user pool
         *
         * @param userPool cognito user pool entity
         * @return updated cognito user pool entity.
         */
        Entity::Cognito::UserPool updateUserPool(Entity::Cognito::UserPool &userPool) const override;

        /**
         * @brief Deletes an existing cognito user pool
         *
         * @param userPoolId cognito user pool ID
         * @throws DatabaseException
         */
        void deleteUserPool(const std::string &userPoolId) const override;

        /**
         * @brief Deletes all existing cognito user pools
         *
         * @return number of user pools deleted
         * @throws DatabaseException
         */
        long deleteAllUserPools() const override;

        /**
         * @brief Check the existence of a cognito user
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @param userName name of the user
         * @return true if a cognito user exists
         * @throws DatabaseException
         */
        bool userExists(const std::string &region, const std::string &userPoolId, const std::string &userName) const override;

        /**
         * @brief Check the existence of a cognito user
         *
         * @param region AWS region name
         * @param userName name of the user
         * @return true if a cognito user exists
         * @throws DatabaseException
         */
        bool userExists(const std::string &region, const std::string &userName) const override;

        /**
         * @brief Create a new cognito user
         *
         * @param user cognito user entity to create
         * @return created cognito user entity.
         */
        Entity::Cognito::User createUser(Entity::Cognito::User &user) const override;

        /**
         * @brief Create or update a cognito user
         *
         * @param user cognito user entity to create
         * @return created cognito user entity.
         */
        Entity::Cognito::User createOrUpdateUser(Entity::Cognito::User &user) const override;

        /**
         * @brief Returns a cognito user entity by primary key
         *
         * @param oid cognito user primary key
         * @return cognito user entity
         * @throws DatabaseException
         */
        Entity::Cognito::User getUserById(bsoncxx::oid oid) const override;

        /**
         * @brief Returns a cognito user entity by primary key
         *
         * @param oid cognito user primary key
         * @return cognito user entity
         * @throws DatabaseException
         */
        Entity::Cognito::User getUserById(const std::string &oid) const override;

        /**
         * @brief Returns a cognito user entity by region, userId and name.
         *
         * @param region aws-mock region.
         * @param userPoolId user pool ID
         * @param userName user name
         * @return cognito user entity
         * @throws DatabaseException
         */
        Entity::Cognito::User getUserByUserName(const std::string &region, const std::string &userPoolId, const std::string &userName) const override;

        /**
         * @brief Count all users
         *
         * @param region aws-mock region.
         * @param userPoolId user pool ID
         * @param groupName group name
         * @return total number of users.
         */
        long countUsers(const std::string &region, const std::string &userPoolId, const std::string &groupName) const override;

        /**
         * @brief Returns a list of cognito users.
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @param prefix name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns sort columns
         * @return list of cognito users
         */
        std::vector<Entity::Cognito::User> listUsers(const std::string &region, const std::string &userPoolId, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Exports a list of cognito users.
         *
         * @param sortColumns sort columns
         * @return list of cognito groups
         */
        std::vector<Entity::Cognito::User> exportUsers(const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Returns a list of cognito users in a given group.
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @param groupName name of the group
         * @return list of cognito users
         */
        std::vector<Entity::Cognito::User> listUsersInGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) const override;

        /**
         * @brief Updates an existing cognito user
         *
         * @param user user entity
         * @return updated cognito user entity.
         */
        Entity::Cognito::User updateUser(Entity::Cognito::User &user) const override;

        /**
         * @brief Deletes an existing cognito user
         *
         * @param user cognito user to delete
         * @return number of users deleted
         * @throws DatabaseException
         */
        long deleteUser(const Entity::Cognito::User &user) const override;

        /**
         * @brief Deletes all existing cognito users
         *
         * @return number of users deleted
         * @throws DatabaseException
         */
        long deleteAllUsers() const override;

        /**
         * @brief Check the existence of a cognito group
         *
         * @param region AWS region name
         * @param groupName group name
         * @return true if cognito group exists
         * @throws DatabaseException
         */
        bool groupExists(const std::string &region, const std::string &groupName) const override;

        /**
         * @brief Returns a cognito user entity by primary key
         *
         * @param oid cognito user primary key
         * @return cognito user entity
         * @throws DatabaseException
         */
        Entity::Cognito::Group getGroupById(bsoncxx::oid oid) const override;

        /**
         * @brief Returns a cognito group entity by region, userPoolId and groupName.
         *
         * @param region AWS region.
         * @param userPoolId user pool ID
         * @param groupName group name
         * @return cognito group entity
         * @throws DatabaseException
         */
        Entity::Cognito::Group getGroupByGroupName(const std::string &region, const std::string &userPoolId, const std::string &groupName) const override;

        /**
         * @brief Create a new cognito group
         *
         * @param group cognito group entity to create
         * @return created cognito group entity.
         */
        Entity::Cognito::Group createGroup(Entity::Cognito::Group &group) const override;

        /**
         * @brief Returns a list of cognito groups.
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @return list of cognito groups
         */
        std::vector<Entity::Cognito::Group> listGroups(const std::string &region, const std::string &userPoolId) const override;

        /**
         * @brief Exports a list of cognito groups.
         *
         * @param sortColumns sort columns
         * @return list of cognito groups
         */
        std::vector<Entity::Cognito::Group> exportGroups(const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Deletes an existing cognito user pool
         *
         * @param region AWS region
         * @param userPoolId cognito user pool ID
         * @param groupName name of the group
         * @return number of groups deleted
         * @throws DatabaseException
         */
        long deleteGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) const override;

        /**
         * @brief Deletes all existing cognito user groups.
         *
         * @param region AWS region
         * @return number of groups deleted
         * @throws DatabaseException
         */
        long deleteAllGroups(const std::string &region) const override;

        /**
         * @brief Check the existence of a client ID
         *
         * @param region AWS region name
         * @param clientId client ID
         * @return true if cognito client ID exists
         * @throws DatabaseException
         */
        bool clientIdExists(const std::string &region, const std::string &clientId) const override;

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Cognito"};

        /**
         * Cognito user pool map
         */
        mutable std::unordered_map<std::string, Entity::Cognito::UserPool> _userPools{};

        /**
         * Cognito user map
         */
        mutable std::unordered_map<std::string, Entity::Cognito::User> _users{};

        /**
         * Cognito group map
         */
        mutable std::unordered_map<std::string, Entity::Cognito::Group> _groups{};

        /**
         * Cognito user pool mutex
         */
        static boost::mutex _userPoolMutex;

        /**
         * Cognito user mutex
         */
        static boost::mutex _userMutex;

        /**
         * Cognito group mutex
         */
        static boost::mutex _groupMutex;
    };

} // namespace Awsmock::Database
