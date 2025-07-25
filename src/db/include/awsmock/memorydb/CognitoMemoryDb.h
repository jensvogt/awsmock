//
// Created by vogje01 on 29/05/2023.
//

#ifndef AWSMOCK_REPOSITORY_COGNITO_MEMORYDB_H
#define AWSMOCK_REPOSITORY_COGNITO_MEMORYDB_H

// C++ standard includes
#include <string>
#include <vector>

// Boost includes
#include <boost/thread/mutex.hpp>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/entity/cognito/Group.h>
#include <awsmock/entity/cognito/User.h>
#include <awsmock/entity/cognito/UserPool.h>
#include <awsmock/repository/Database.h>
#include <awsmock/utils/SortColumn.h>

namespace AwsMock::Database {

    /**
     * @brief Cognito in-memory database.
     *
     * @par
     * Provides an in-memory database using a simple hash map. The key is a randomly generated UUID.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class CognitoMemoryDb {

      public:

        /**
         * @brief Constructor
         */
        CognitoMemoryDb() = default;

        /**
         * @brief Singleton instance
         */
        static CognitoMemoryDb &instance() {
            static CognitoMemoryDb cognitoMemoryDb;
            return cognitoMemoryDb;
        }

        /**
         * @brief Check existence of cognito user pool
         *
         * @param region AWS region name
         * @param name cognito user pool name
         * @return true if cognito already exists
         * @throws DatabaseException
         */
        bool UserPoolExists(const std::string &region, const std::string &name);

        /**
         * @brief Check existence of cognito user pool
         *
         * @param userPoolId user pool userPoolId
         * @return true if cognito user pool exists
         * @throws DatabaseException
         */
        bool UserPoolExists(const std::string &userPoolId);

        /**
         * @brief Create a new cognito user pool
         *
         * @param userPool cognito user pool entity to create
         * @return created cognito user pool entity.
         */
        Entity::Cognito::UserPool CreateUserPool(const Entity::Cognito::UserPool &userPool);

        /**
         * @brief Returns a cognito user pool entity by primary key
         *
         * @param oid cognito user pool primary key
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        Entity::Cognito::UserPool GetUserPoolByOid(const std::string &oid);

        /**
         * @brief Returns a cognito user pool entity by primary key
         *
         * @param userPoolId cognito user pool ID
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        Entity::Cognito::UserPool GetUserPoolByUserPoolId(const std::string &userPoolId);

        /**
         * @brief Returns a cognito user pool entity by client Id
         *
         * @param clientId cognito client ID
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        Entity::Cognito::UserPool GetUserPoolByClientId(const std::string &clientId);

        /**
         * @brief Returns a cognito user pool entity by region and name
         *
         * @param region AWS region
         * @param name cognito user pool name
         * @return cognito entity
         * @throws DatabaseException
         */
        Entity::Cognito::UserPool GetUserPoolByRegionName(const std::string &region, const std::string &name);

        /**
         * @brief Count all user pools
         *
         * @param region aws-mock region.
         * @return total number of user pools.
         */
        long CountUserPools(const std::string &region = {}) const;

        /**
         * @brief Returns a list of cognito user pools.
         *
         * @param region AWS region name
         * @return list of cognito user pools
         */
        std::vector<Entity::Cognito::UserPool> ListUserPools(const std::string &region);

        /**
         * @brief Exports a list of cognito user pools.
         *
         * @param sortColumns sort columns
         * @return list of cognito user pools
         */
        std::vector<Entity::Cognito::UserPool> ExportUserPools(const std::vector<SortColumn> &sortColumns) const;

        /**
         * @brief Updates an existing cognito user pool
         *
         * @param userPool cognito user pool entity
         * @return updated cognito user pool entity.
         */
        Entity::Cognito::UserPool UpdateUserPool(const Entity::Cognito::UserPool &userPool);

        /**
         * @brief Deletes an existing cognito user pool
         *
         * @param userPoolId cognito user pool ID
         * @throws DatabaseException
         */
        void DeleteUserPool(const std::string &userPoolId);

        /**
         * @brief Deletes all existing cognito user pools
         *
         * @throws DatabaseException
         */
        void DeleteAllUserPools();

        /**
         * @brief Check existence of cognito user
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @param userName name of the user
         * @return true if cognito user exists
         * @throws DatabaseException
         */
        bool UserExists(const std::string &region, const std::string &userPoolId, const std::string &userName);

        /**
         * Check existence of cognito user
         *
         * @param region AWS region name
         * @param userName name of the user
         * @return true if cognito user exists
         * @throws DatabaseException
         */
        bool UserExists(const std::string &region, const std::string &userName);

        /**
         * @brief Create a new cognito user
         *
         * @param user cognito user entity to create
         * @return created cognito user entity.
         */
        Entity::Cognito::User CreateUser(const Entity::Cognito::User &user);

        /**
         * @brief Returns a cognito user entity by primary key
         *
         * @param oid cognito user primary key
         * @return cognito user entity
         * @throws DatabaseException
         */
        Entity::Cognito::User GetUserByOid(const std::string &oid);

        /**
         * @brief Returns a cognito user entity by region, userId and name.
         *
         * @param region aws-mock region.
         * @param userPoolId user pool ID
         * @param userName user name
         * @return cognito user entity
         * @throws DatabaseException
         */
        Entity::Cognito::User GetUserByUserName(const std::string &region, const std::string &userPoolId, const std::string &userName);

        /**
         * @brief Count all users
         *
         * @param region aws-mock region.
         * @param userPoolId user pool ID
         * @param groupName group name
         * @return total number of users.
         */
        long CountUsers(const std::string &region = {}, const std::string &userPoolId = {}, const std::string &groupName = {}) const;

        /**
         * @brief Returns a list of cognito users.
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @return list of cognito users
         */
        std::vector<Entity::Cognito::User> ListUsers(const std::string &region = {}, const std::string &userPoolId = {}) const;

        /**
         * @brief Exports a list of cognito users.
         *
         * @param sortColumns sort columns
         * @return list of cognito groups
         */
        std::vector<Entity::Cognito::User> ExportUsers(const std::vector<SortColumn> &sortColumns) const;

        /**
         * @brief Returns a list of cognito users in given group.
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @param groupName name of the group
         * @return list of cognito users
         */
        std::vector<Entity::Cognito::User> ListUsersInGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName);

        /**
         * @brief Updates an existing cognito user
         *
         * @param user user entity
         * @return updated cognito user entity.
         */
        Entity::Cognito::User UpdateUser(const Entity::Cognito::User &user);

        /**
         * @brief Deletes an existing cognito users
         *
         * @param user cognito user to delete
         * @throws DatabaseException
         */
        void DeleteUser(const Entity::Cognito::User &user);

        /**
         * @brief Deletes all existing cognito users
         *
         * @throws DatabaseException
         */
        void DeleteAllUsers();

        /**
         * @brief Check existence of cognito group
         *
         * @param region AWS region name
         * @param groupName group name
         * @return true if cognito group exists
         * @throws DatabaseException
         */
        bool GroupExists(const std::string &region, const std::string &groupName);

        /**
         * @brief Returns a cognito group entity by region, userPoolId and groupName.
         *
         * @param region AWS region.
         * @param userPoolId user pool ID
         * @param groupName group name
         * @return cognito group entity
         * @throws DatabaseException
         */
        Entity::Cognito::Group GetGroupByGroupName(const std::string &region, const std::string &userPoolId, const std::string &groupName);

        /**
         * @brief Create a new cognito group
         *
         * @param group cognito group entity to create
         * @return created cognito group entity.
         */
        Entity::Cognito::Group CreateGroup(const Entity::Cognito::Group &group);

        /**
         * @brief Returns a list of cognito groups.
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @return list of cognito groups
         */
        std::vector<Entity::Cognito::Group> ListGroups(const std::string &region = {}, const std::string &userPoolId = {});

        /**
         * @brief Exports a list of cognito groups.
         *
         * @param sortColumns sort columns
         * @return list of cognito groups
         */
        std::vector<Entity::Cognito::Group> ExportGroups(const std::vector<SortColumn> &sortColumns);

        /**
         * @brief Deletes an existing cognito user pool
         *
         * @param region AWS region
         * @param userPoolId cognito user pool ID
         * @param groupName name of the group
         * @throws DatabaseException
         */
        void DeleteGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName);

        /**
         * @brief Deletes all existing cognito user groups.
         *
         * @param region AWS region
         * @throws DatabaseException
         */
        void DeleteAllGroups(const std::string &region = {});

        /**
         * @brief Check existence of a client ID
         *
         * @param region AWS region name
         * @param clientId client ID
         * @return true if cognito client ID exists
         * @throws DatabaseException
         */
        bool ClientIdExists(const std::string &region, const std::string &clientId);

      private:

        /**
         * Cognito user pool map
         */
        std::map<std::string, Entity::Cognito::UserPool> _userPools{};

        /**
         * Cognito user map
         */
        std::map<std::string, Entity::Cognito::User> _users{};

        /**
         * Cognito group map
         */
        std::map<std::string, Entity::Cognito::Group> _groups{};

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

}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_COGNITO_MEMORYDB_H
