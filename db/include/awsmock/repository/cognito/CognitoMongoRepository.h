//
// Created by vogje01 on 29/11/2023.
//

#pragma once

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/repository/Database.h>
#include <awsmock/repository/cognito/ICognitoRepository.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Cognito MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class CognitoMongoRepository : public ICognitoRepository {

    public:
        /**
         * @brief Constructor
         */
        CognitoMongoRepository() = default;

        /**
         * @brief Singleton instance
         */
        static CognitoMongoRepository &instance() {
            static CognitoMongoRepository instance;
            return instance;
        }

        /**
         * @brief Check the existence of the cognito user pool
         *
         * @param region AWS region name
         * @param name AWS function
         * @return true if cognito user pool exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool userPoolExists(const std::string &region, const std::string &name) const override;

        /**
         * @brief Check the existence of the cognito user pool
         *
         * @param userPoolId user pool userPoolId
         * @return true if cognito user pool exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool userPoolExists(const std::string &userPoolId) const override;

        /**
         * @brief Create a new cognito user pool
         *
         * @param userPool cognito user pool entity to create
         * @return created cognito entity.
         */
        [[nodiscard]]
        Entity::Cognito::UserPool createUserPool(Entity::Cognito::UserPool &userPool) const override;

        /**
         * @brief Count all user pools
         *
         * @param region aws-mock region.
         * @return total number of user pools.
         */
        [[nodiscard]]
        long countUserPools(const std::string &region) const override;

        /**
         * @brief Updates an existing cognito user pool
         *
         * @param userPool user pool entity
         * @return updated cognito user pool entity.
         */
        [[nodiscard]]
        Entity::Cognito::UserPool updateUserPool(Entity::Cognito::UserPool &userPool) const override;

        /**
         * @brief Creates a new user pool or updates an existing user pool
         *
         * @param userPool user pool entity
         * @return created or updated Cognito user pool entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Cognito::UserPool createOrUpdateUserPool(Entity::Cognito::UserPool &userPool) const override;

        /**
         * @brief Returns a cognito user pool entity by primary key
         *
         * @param oid cognito user pool primary key
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Cognito::UserPool getUserPoolById(bsoncxx::oid oid) const override;

        /**
         * @brief Returns a cognito user pool entity by primary key
         *
         * @param userPoolId cognito user pool ID
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Cognito::UserPool getUserPoolByUserPoolId(const std::string &userPoolId) const override;

        /**
         * @brief Returns a cognito user pool entity by client Id
         *
         * @param clientId cognito client ID
         * @return cognito user pool entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Cognito::UserPool getUserPoolByClientId(const std::string &clientId) const override;

        /**
         * @brief Returns a cognito user pool entity by region and name
         *
         * @param region AWS region
         * @param name cognito user pool name
         * @return cognito entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Cognito::UserPool getUserPoolByRegionName(const std::string &region, const std::string &name) const override;

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
        std::vector<Entity::Cognito::UserPool> listUserPools(const std::string &region, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Exports a list of cognito user pools.
         *
         * @param sortColumns sort columns
         * @return list of cognito user pools
         */
        [[nodiscard]]
        std::vector<Entity::Cognito::UserPool> exportUserPools(const std::vector<SortColumn> &sortColumns) const override;

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
        [[nodiscard]]
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
        [[nodiscard]]
        bool userExists(const std::string &region, const std::string &userPoolId, const std::string &userName) const override;

        /**
         * @brief Check the existence of a cognito user
         *
         * @param region AWS region name
         * @param userName name of the user
         * @return true if cognito user exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool userExists(const std::string &region, const std::string &userName) const override;

        /**
         * @brief Create a new cognito user
         *
         * @param user cognito user entity to create
         * @return created cognito user entity.
         */
        [[nodiscard]]
        Entity::Cognito::User createUser(Entity::Cognito::User &user) const override;

        /**
         * @brief Returns a cognito user entity by primary key
         *
         * @param oid cognito user primary key
         * @return cognito user entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Cognito::User getUserById(bsoncxx::oid oid) const override;

        /**
         * @brief Returns a cognito user entity by primary key
         *
         * @param oid cognito user primary key
         * @return cognito user entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Cognito::User getUserById(const std::string &oid) const override;

        /**
         * @brief Returns a cognito user entity by region, userId, and name.
         *
         * @param region aws-mock region.
         * @param userPoolId user pool ID
         * @param userName user name
         * @return cognito user entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Cognito::User getUserByUserName(const std::string &region, const std::string &userPoolId, const std::string &userName) const override;

        /**
         * @brief Count all users
         *
         * @param region aws-mock region.
         * @param userPoolId user pool ID
         * @param groupName group name
         * @return total number of users.
         */
        [[nodiscard]]
        long countUsers(const std::string &region, const std::string &userPoolId, const std::string &groupName) const override;

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
        std::vector<Entity::Cognito::User> listUsers(const std::string &region, const std::string &userPoolId, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Exports a list of cognito users.
         *
         * @param sortColumns sort columns
         * @return list of cognito groups
         */
        [[nodiscard]]
        std::vector<Entity::Cognito::User> exportUsers(const std::vector<SortColumn> &sortColumns) const override;

        /**
         * @brief Returns a list of cognito users in the given group.
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @param groupName name of the group
         * @return list of cognito users
         */
        [[nodiscard]]
        std::vector<Entity::Cognito::User> listUsersInGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) const override;

        /**
         * @brief Updates an existing cognito user
         *
         * @param user user entity
         * @return updated cognito user entity.
         */
        [[nodiscard]]
        Entity::Cognito::User updateUser(Entity::Cognito::User &user) const override;

        /**
         * @brief Creates a new user or updates an existing user
         *
         * @param user user entity
         * @return created or updated Cognito user entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Cognito::User createOrUpdateUser(Entity::Cognito::User &user) const override;

        /**
         * @brief Deletes an existing cognito user
         *
         * @param user cognito user to delete
         * @return number of users deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteUser(const Entity::Cognito::User &user) const override;

        /**
         * @brief Deletes all existing cognito users
         *
         * @return number of users deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteAllUsers() const override;

        /**
         * @brief Check the existence of a cognito group
         *
         * @param region AWS region name
         * @param groupName group name
         * @return true if cognito group exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool groupExists(const std::string &region, const std::string &groupName) const override;

        /**
         * @brief Returns a cognito group entity by primary key
         *
         * @param oid cognito group primary key
         * @return cognito group entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Cognito::Group getGroupById(bsoncxx::oid oid) const override;

        /**
         * @brief Returns a cognito group entity by region, userPoolId, and groupName.
         *
         * @param region AWS region.
         * @param userPoolId user pool ID
         * @param groupName group name
         * @return cognito group entity
         * @throws DatabaseException
         */
        [[nodiscard]]
        Entity::Cognito::Group getGroupByGroupName(const std::string &region, const std::string &userPoolId, const std::string &groupName) const override;

        /**
         * @brief Create a new cognito group
         *
         * @param group cognito group entity to create
         * @return created cognito group entity.
         */
        [[nodiscard]]
        Entity::Cognito::Group createGroup(Entity::Cognito::Group &group) const override;

        /**
         * @brief Returns a list of cognito groups.
         *
         * @param region AWS region name
         * @param userPoolId user pool ID
         * @return list of cognito groups
         */
        [[nodiscard]]
        std::vector<Entity::Cognito::Group> listGroups(const std::string &region, const std::string &userPoolId) const override;

        /**
         * @brief Exports a list of cognito groups.
         *
         * @param sortColumns sort columns
         * @return list of cognito groups
         */
        [[nodiscard]]
        std::vector<Entity::Cognito::Group> exportGroups(const std::vector<SortColumn> &sortColumns) const override;

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
        long deleteGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) const override;

        /**
         * @brief Deletes all existing cognito user groups.
         *
         * @param region AWS region
         * @return number of groups deleted
         * @throws DatabaseException
         */
        [[nodiscard]]
        long deleteAllGroups(const std::string &region) const override;

        /**
         * @brief Check the existence of a client ID
         *
         * @param region AWS region name
         * @param clientId client ID
         * @return true if cognito client ID exists
         * @throws DatabaseException
         */
        [[nodiscard]]
        bool clientIdExists(const std::string &region, const std::string &clientId) const override;

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Cognito"};

        static constexpr auto DATABASE_NAME = "awsmock";
        static constexpr auto USERPOOL_COLLECTION = "cognito_userpool";
        static constexpr auto USER_COLLECTION = "cognito_user";
        static constexpr auto GROUP_COLLECTION = "cognito_group";

        [[nodiscard]]
        static mongocxx::collection userpoolCollection() {
            const auto entry = Database::instance().client();
            return (*entry)[Database::instance().databaseName()][USERPOOL_COLLECTION];
        }

        [[nodiscard]]
        static mongocxx::collection userCollection() {
            const auto entry = ConnectionPool::instance().GetConnection();
            return (*entry)[Database::instance().databaseName()][USER_COLLECTION];
        }

        [[nodiscard]]
        static mongocxx::collection groupCollection() {
            const auto entry = ConnectionPool::instance().GetConnection();
            return (*entry)[Database::instance().databaseName()][GROUP_COLLECTION];
        }
    };

} // namespace Awsmock::Database
