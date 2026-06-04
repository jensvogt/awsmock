//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <optional>
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/cognito/User.h>
#include <awsmock/entity/cognito/UserPool.h>
#include <awsmock/entity/sqs/Message.h>
#include <awsmock/entity/sqs/Queue.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for Cognito repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * Cognito-related data.
     */
    class ICognitoRepository {

      public:

        /**
         * @brief Virtual destructor for the ISQSRepository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~ICognitoRepository() = default;

        /**
         * @brief Check the existence of the cognito user pool
         *
         * @param region AWS region name
         * @param name AWS function
         * @return true if cognito user pool exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool userPoolExists(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Check the existence of the cognito user pool
         *
         * @param userPoolId user pool userPoolId
         * @return true if cognito user pool exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool userPoolExists(const std::string &userPoolId) const = 0;

        /**
         * @brief Create a new cognito user pool
         *
         * @param userPool cognito user pool entity to create
         * @return created cognito entity.
         */
        [[nodiscard]]
        virtual Entity::Cognito::UserPool createUserPool(Entity::Cognito::UserPool &userPool) const = 0;

        /**
         * @brief Count all user pools
         *
         * @param region aws-mock region.
         * @return total number of user pools.
         */
        [[nodiscard]]
        virtual long countUserPools(const std::string &region) const = 0;

        /**
         * @brief Updates an existing cognito user pool
         *
         * @param userPool user pool entity
         * @return updated cognito user pool entity.
         */
        [[nodiscard]]
        virtual Entity::Cognito::UserPool updateUserPool(Entity::Cognito::UserPool &userPool) const = 0;

        /**
         * @brief Creates a new user pool or updates an existing user pool
         *
         * @param userPool user pool entity
         * @return created or updated Cognito user pool entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Cognito::UserPool createOrUpdateUserPool(Entity::Cognito::UserPool &userPool) const = 0;

        /**
         * @brief Returns a cognito user pool entity by primary key
         *
         * @param oid cognito user pool primary key
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Cognito::UserPool getUserPoolById(bsoncxx::oid oid) const = 0;

        /**
         * @brief Returns a cognito user pool entity by primary key
         *
         * @param userPoolId cognito user pool ID
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Cognito::UserPool getUserPoolByUserPoolId(const std::string &userPoolId) const = 0;

        /**
         * @brief Returns a cognito user pool entity by client Id
         *
         * @param clientId cognito client ID
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Cognito::UserPool getUserPoolByClientId(const std::string &clientId) const = 0;

        /**
         * @brief Returns a cognito user pool entity by region and name
         *
         * @param region AWS region
         * @param name cognito user pool name
         * @return cognito entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Cognito::UserPool getUserPoolByRegionName(const std::string &region, const std::string &name) const = 0;

        /**
         * @brief Returns a list of cognito user pools.
         *
         * @param region AWS region name
         * @param prefix user pool name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns columns list of sort columns
         * @return list of cognito user pools
         */
        [[nodiscard]]
        virtual std::vector<Entity::Cognito::UserPool> listUserPools(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Exports a list of cognito user pools.
         *
         * @param sortColumns sort columns
         * @return list of cognito user pools
         */
        [[nodiscard]]
        virtual std::vector<Entity::Cognito::UserPool> exportUserPools(const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Deletes an existing cognito user pool
         *
         * @param userPoolId cognito user pool ID
         * @throws DatabaseException
         */
        virtual void deleteUserPool(const std::string &userPoolId) const = 0;

        /**
         * @brief Deletes all existing cognito user pools
         *
         * @return number of user pools deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllUserPools() const = 0;

        /**
         * @brief Check the existence of a cognito user
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @param userName name of the user
         * @return true if the cognito user exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool userExists(const std::string &region, const std::string &userPoolId, const std::string &userName) const = 0;

        /**
         * Check the existence of a cognito user
         *
         * @param region AWS region name
         * @param userName name of the user
         * @return true if a cognito user exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool userExists(const std::string &region, const std::string &userName) const = 0;

        /**
         * @brief Create a new cognito user
         *
         * @param user cognito user entity to create
         * @return created cognito user entity.
         */
        [[nodiscard]]
        virtual Entity::Cognito::User createUser(Entity::Cognito::User &user) const = 0;

        /**
         * @brief Returns a cognito user entity by primary key
         *
         * @param oid cognito user primary key
         * @return cognito user entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Cognito::User getUserById(bsoncxx::oid oid) const = 0;

        /**
         * @brief Returns a cognito user entity by primary key
         *
         * @param oid cognito user primary key
         * @return cognito user entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Cognito::User getUserById(const std::string &oid) const = 0;

        /**
         * @brief Returns a cognito user entity by region, userId and name.
         *
         * @param region aws-mock region.
         * @param userPoolId user pool ID
         * @param userName user name
         * @return cognito user entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Cognito::User getUserByUserName(const std::string &region, const std::string &userPoolId, const std::string &userName) const = 0;

        /**
         * @brief Count all users
         *
         * @param region aws-mock region.
         * @param userPoolId user pool ID
         * @param groupName group name
         * @return total number of users.
         */
        [[nodiscard]]
        virtual long countUsers(const std::string &region, const std::string &userPoolId, const std::string &groupName) const = 0;

        /**
         * @brief Returns a list of cognito users.
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @param prefix user pool name prefix
         * @param pageSize page size
         * @param pageIndex page index
         * @param sortColumns columns list of sort columns
         * @return list of cognito users
         */
        [[nodiscard]]
        virtual std::vector<Entity::Cognito::User> listUsers(const std::string &region, const std::string &userPoolId, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Exports a list of cognito users.
         *
         * @param sortColumns sort columns
         * @return list of cognito groups
         */
        [[nodiscard]]
        virtual std::vector<Entity::Cognito::User> exportUsers(const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Returns a list of cognito users in a given group.
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @param groupName name of the group
         * @return list of cognito users
         */
        [[nodiscard]]
        virtual std::vector<Entity::Cognito::User> listUsersInGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) const = 0;

        /**
         * @brief Updates an existing cognito user
         *
         * @param user user entity
         * @return updated cognito user entity.
         */
        [[nodiscard]]
        virtual Entity::Cognito::User updateUser(Entity::Cognito::User &user) const = 0;

        /**
         * @brief Creates a new user or updates an existing user
         *
         * @param user user entity
         * @return created or updated Cognito user entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Cognito::User createOrUpdateUser(Entity::Cognito::User &user) const = 0;

        /**
         * @brief Deletes an existing cognito user
         *
         * @param user cognito user to delete
         * @return number of users deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteUser(const Entity::Cognito::User &user) const = 0;

        /**
         * @brief Deletes all existing cognito users
         *
         * @return number of users deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllUsers() const = 0;

        /**
         * @brief Check the existence of a cognito group
         *
         * @param region AWS region name
         * @param groupName group name
         * @return true if cognito group exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool groupExists(const std::string &region, const std::string &groupName) const = 0;

        /**
         * @brief Returns a cognito group entity by primary key
         *
         * @param oid cognito group primary key
         * @return cognito group entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Cognito::Group getGroupById(bsoncxx::oid oid) const = 0;

        /**
         * @brief Returns a cognito group entity by region, userPoolId and groupName.
         *
         * @param region AWS region.
         * @param userPoolId user pool ID
         * @param groupName group name
         * @return cognito group entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual Entity::Cognito::Group getGroupByGroupName(const std::string &region, const std::string &userPoolId, const std::string &groupName) const = 0;

        /**
         * @brief Create a new cognito group
         *
         * @param group cognito group entity to create
         * @return created cognito group entity.
         */
        [[nodiscard]]
        virtual Entity::Cognito::Group createGroup(Entity::Cognito::Group &group) const = 0;

        /**
         * @brief Returns a list of cognito groups.
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @return list of cognito groups
         */
        [[nodiscard]]
        virtual std::vector<Entity::Cognito::Group> listGroups(const std::string &region, const std::string &userPoolId) const = 0;

        /**
         * @brief Exports a list of cognito groups.
         *
         * @param sortColumns sort columns
         * @return list of cognito groups
         */
        [[nodiscard]]
        virtual std::vector<Entity::Cognito::Group> exportGroups(const std::vector<SortColumn> &sortColumns) const = 0;

        /**
         * @brief Deletes an existing cognito user group.
         *
         * @param region AWS region
         * @param userPoolId cognito user pool ID
         * @param groupName name of the group
         * @return number of groups deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) const = 0;

        /**
         * @brief Deletes all existing cognito user groups.
         *
         * @param region AWS region
         * @return number of groups deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual long deleteAllGroups(const std::string &region) const = 0;

        /**
         * @brief Check the existence of a client ID
         *
         * @param region AWS region name
         * @param clientId client ID
         * @return true if cognito client ID exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        virtual bool clientIdExists(const std::string &region, const std::string &clientId) const = 0;
    };

}// namespace Awsmock::Database