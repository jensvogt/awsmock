//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/cognito/CognitoMongoRepository.h>

namespace Awsmock::Database {

    bool CognitoMongoRepository::userPoolExists(const std::string &region, const std::string &name) const {

        try {

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            return col.count_documents(make_document(kvp("region", region), kvp("name", name)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    bool CognitoMongoRepository::userPoolExists(const std::string &userPoolId) const {

        try {
            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            return col.count_documents(make_document(kvp("userPoolId", userPoolId)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::UserPool CognitoMongoRepository::createUserPool(Entity::Cognito::UserPool &userPool) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            const auto result = col.insert_one(userPool.ToDocument());
            log_trace << "User pool created, oid: " << result->inserted_id().get_oid().value.to_string();
            userPool.oid = result->inserted_id().get_oid().value.to_string();
            return userPool;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::UserPool CognitoMongoRepository::getUserPoolById(bsoncxx::oid oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            const auto mResult = col.find_one(make_document(kvp("_id", oid)));
            if (!mResult) {
                log_error << "Database exception: Cognito not found ";
                throw Core::DatabaseException("Database exception, Cognito not found ");
            }

            Entity::Cognito::UserPool result;
            result.FromDocument(mResult->view());
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::UserPool CognitoMongoRepository::getUserPoolByUserPoolId(const std::string &userPoolId) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            const auto mResult = col.find_one(make_document(kvp("userPoolId", userPoolId)));
            if (!mResult) {
                log_error << "Database exception: user pool not found ";
                throw Core::DatabaseException("Database exception, user pool not found ");
            }

            Entity::Cognito::UserPool result;
            result.FromDocument(mResult->view());
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::UserPool CognitoMongoRepository::getUserPoolByClientId(const std::string &clientId) const {

        try {
            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            const auto mResult = col.find_one(make_document(kvp("userPoolClients", make_document(kvp("$elemMatch", make_document(kvp("clientId", clientId)))))));
            if (!mResult) {
                log_error << "Database exception: user pool not found ";
                throw Core::DatabaseException("Database exception, user pool not found ");
            }

            Entity::Cognito::UserPool result;
            result.FromDocument(mResult->view());
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::UserPool CognitoMongoRepository::getUserPoolByRegionName(const std::string &region, const std::string &name) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            const auto mResult = col.find_one(make_document(kvp("region", region), kvp("name", name)));
            if (!mResult) {
                log_error << "Database exception: Cognito not found ";
                throw Core::DatabaseException("Database exception, Cognito not found ");
            }

            Entity::Cognito::UserPool result;
            result.FromDocument(mResult->view());
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::UserPool CognitoMongoRepository::createOrUpdateUserPool(Entity::Cognito::UserPool &userPool) const {

        if (userPoolExists(userPool.region, userPool.name)) {

            return updateUserPool(userPool);
        }
        return createUserPool(userPool);
    }

    Entity::Cognito::UserPool CognitoMongoRepository::updateUserPool(Entity::Cognito::UserPool &userPool) const {


        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            if (const auto mResult = col.find_one_and_update(make_document(kvp("region", userPool.region), kvp("name", userPool.name)), userPool.ToDocument(), opts)) {
                log_trace << "Cognito user pool updated: " << userPool.ToString();
                userPool.FromDocument(mResult->view());
                return userPool;
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    std::vector<Entity::Cognito::UserPool> CognitoMongoRepository::listUserPools(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        std::vector<Entity::Cognito::UserPool> userPools;

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];

            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                document sort = {};
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                }
                opts.sort(sort.extract());
            }
            if (pageSize > 0) {
                opts.skip(pageSize * pageIndex);
                opts.limit(pageSize);
            }

            document query = {};
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!prefix.empty()) {
                query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
            }

            for (auto userPoolCursor = col.find(query.extract(), opts); const auto &userPool: userPoolCursor) {
                Entity::Cognito::UserPool result;
                result.FromDocument(userPool);
                userPools.push_back(result);
            }
            return userPools;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    std::vector<Entity::Cognito::UserPool> CognitoMongoRepository::exportUserPools(const std::vector<SortColumn> &sortColumns) const {

        try {

            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                document sort = {};
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                }
                opts.sort(sort.extract());
            }

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            std::vector<Entity::Cognito::UserPool> userPools;
            for (auto userPoolCursor = col.find({}, opts); auto userPool: userPoolCursor) {
                Entity::Cognito::UserPool result;
                result.FromDocument(userPool);
                userPools.push_back(result);
            }
            log_trace << "Got user pool list, size:" << userPools.size();
            return userPools;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long CognitoMongoRepository::countUserPools(const std::string &region) const {

        try {
            long count = 0;

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            count = col.count_documents(query.view());
            log_trace << "User pool count: " << count;
            return count;

        } catch (mongocxx::exception::system_error &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    void CognitoMongoRepository::deleteUserPool(const std::string &userPoolId) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            const auto result = col.delete_many(make_document(kvp("userPoolId", userPoolId)));
            log_debug << "User pool deleted, userPoolId: " << userPoolId << " count: " << result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long CognitoMongoRepository::deleteAllUserPools() const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USERPOOL_COLLECTION];
            const auto result = col.delete_many({});
            log_debug << "All cognito user pools deleted, count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    bool CognitoMongoRepository::userExists(const std::string &region, const std::string &userPoolId, const std::string &userName) const {

        try {

            // Set limit to 1 (Very important for performance!)
            mongocxx::options::count options;
            options.limit(1);

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            return col.count_documents(make_document(kvp("region", region), kvp("userPoolId", userPoolId), kvp("userName", userName)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    bool CognitoMongoRepository::userExists(const std::string &region, const std::string &userName) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            const int64_t count = col.count_documents(make_document(kvp("region", region), kvp("userName", userName)));
            log_trace << "Cognito user exists: " << std::boolalpha << count;
            return count > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::User CognitoMongoRepository::createUser(Entity::Cognito::User &user) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            const auto result = col.insert_one(user.ToDocument());
            log_trace << "User created, oid: " << result->inserted_id().get_oid().value.to_string();
            user.oid = result->inserted_id().get_oid().value.to_string();
            return user;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::User CognitoMongoRepository::getUserById(bsoncxx::oid oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            const auto mResult = col.find_one(make_document(kvp("_id", oid)));
            if (!mResult) {
                log_error << "Database exception: user not found ";
                throw Core::DatabaseException("Database exception, user not found ");
            }

            Entity::Cognito::User result;
            result.FromDocument(mResult->view());
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::User CognitoMongoRepository::getUserByUserName(const std::string &region, const std::string &userPoolId, const std::string &userName) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            const auto mResult = col.find_one(make_document(kvp("region", region), kvp("userPoolId", userPoolId), kvp("userName", userName)));
            if (!mResult) {
                log_error << "Database exception: user not found ";
                throw Core::DatabaseException("Database exception, user not found ");
            }

            Entity::Cognito::User result;
            result.FromDocument(mResult->view());
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::User CognitoMongoRepository::getUserById(const std::string &oid) const {
        return getUserById(bsoncxx::oid(oid));
    }

    long CognitoMongoRepository::countUsers(const std::string &region, const std::string &userPoolId, const std::string &groupName) const {

        try {

            long count = 0;

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!userPoolId.empty()) {
                query.append(kvp("userPoolId", userPoolId));
            }
            if (!groupName.empty()) {
                query.append(kvp("groups.groupName", groupName));
            }

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            count = col.count_documents(query.view());

            log_trace << "User count: " << count;
            return count;

        } catch (mongocxx::exception::system_error &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    std::vector<Entity::Cognito::User> CognitoMongoRepository::listUsers(const std::string &region, const std::string &userPoolId, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        try {
            std::vector<Entity::Cognito::User> users;

            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                document sort = {};
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                }
                opts.sort(sort.extract());
            }

            if (pageSize > 0) {
                opts.skip(pageSize * pageIndex);
                opts.limit(pageSize);
            }

            document query = {};
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!userPoolId.empty()) {
                query.append(kvp("userPoolId", userPoolId));
            }
            if (!prefix.empty()) {
                query.append(kvp("userName", make_document(kvp("$regex", "^" + prefix))));
            }

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            for (auto userCursor = col.find(query.extract(), opts); const auto &user: userCursor) {
                Entity::Cognito::User result;
                result.FromDocument(user);
                users.push_back(result);
            }
            return users;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    std::vector<Entity::Cognito::User> CognitoMongoRepository::exportUsers(const std::vector<SortColumn> &sortColumns) const {

        try {
            document query;
            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                document sort = {};
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                }
                opts.sort(sort.extract());
            }

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            std::vector<Entity::Cognito::User> users;
            for (auto userCursor = col.find(query.extract(), opts); auto user: userCursor) {
                Entity::Cognito::User result;
                result.FromDocument(user);
                users.push_back(result);
            }
            return users;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    std::vector<Entity::Cognito::User> CognitoMongoRepository::listUsersInGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            auto userCursor = col.find(make_document(kvp("region", region), kvp("userPoolId", userPoolId), kvp("groups", make_document(kvp("$elemMatch", make_document(kvp("groupName", groupName)))))));
            std::vector<Entity::Cognito::User> users;
            for (auto user: userCursor) {
                Entity::Cognito::User result;
                result.FromDocument(user);
                users.push_back(result);
            }
            return users;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::User CognitoMongoRepository::updateUser(Entity::Cognito::User &user) const {


        mongocxx::options::find_one_and_update opts{};
        opts.return_document(mongocxx::options::return_document::k_after);

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            if (const auto mResult = col.find_one_and_update(make_document(kvp("region", user.region), kvp("userPoolId", user.userPoolId), kvp("userName", user.userName)), user.ToDocument(), opts)) {
                log_trace << "Cognito user updated: " << user.ToString();
                user.FromDocument(mResult->view());
                return user;
            }
            return {};

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::User CognitoMongoRepository::createOrUpdateUser(Entity::Cognito::User &user) const {

        if (userExists(user.region, user.userPoolId, user.userName)) {

            return updateUser(user);
        }
        return createUser(user);
    }

    long CognitoMongoRepository::deleteUser(const Entity::Cognito::User &user) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            const auto result = col.delete_many(make_document(kvp("region", user.region), kvp("userPoolId", user.userPoolId), kvp("userName", user.userName)));
            log_debug << "User deleted, userName: " << user.userName << " count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long CognitoMongoRepository::deleteAllUsers() const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][USER_COLLECTION];
            const auto result = col.delete_many({});
            log_debug << "All cognito users deleted, count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    bool CognitoMongoRepository::groupExists(const std::string &region, const std::string &groupName) const {

        try {

            // Set limit to 1
            mongocxx::options::count options;
            options.limit(1);

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][GROUP_COLLECTION];
            return col.count_documents(make_document(kvp("region", region), kvp("groupName", groupName)), options) > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::Group CognitoMongoRepository::getGroupById(bsoncxx::oid oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][GROUP_COLLECTION];
            const auto mResult = col.find_one(make_document(kvp("_id", oid)));
            if (!mResult) {
                log_error << "Database exception: Cognito not found ";
                throw Core::DatabaseException("Database exception, Cognito not found ");
            }

            Entity::Cognito::Group result;
            result.FromDocument(mResult->view());
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::Group CognitoMongoRepository::getGroupByGroupName(const std::string &region, const std::string &userPoolId, const std::string &groupName) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][GROUP_COLLECTION];
            const auto mResult = col.find_one(make_document(kvp("region", region), kvp("userPoolId", userPoolId), kvp("groupName", groupName)));
            if (!mResult) {
                log_error << "Database exception: group not found ";
                throw Core::DatabaseException("Database exception, group not found ");
            }

            Entity::Cognito::Group result;
            result.FromDocument(mResult->view());
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::Cognito::Group CognitoMongoRepository::createGroup(Entity::Cognito::Group &group) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][GROUP_COLLECTION];
            const auto result = col.insert_one(group.ToDocument());
            log_trace << "Cognito group created, oid: " << result->inserted_id().get_oid().value.to_string();
            group.oid = result->inserted_id().get_oid().value.to_string();
            return group;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    std::vector<Entity::Cognito::Group> CognitoMongoRepository::listGroups(const std::string &region, const std::string &userPoolId) const {

        try {

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!userPoolId.empty()) {
                query.append(kvp("userPoolId", userPoolId));
            }

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][GROUP_COLLECTION];
            std::vector<Entity::Cognito::Group> groups;
            for (auto groupCursor = col.find(query.view()); auto group: groupCursor) {
                Entity::Cognito::Group result;
                result.FromDocument(group);
                groups.push_back(result);
            }
            return groups;
        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    std::vector<Entity::Cognito::Group> CognitoMongoRepository::exportGroups(const std::vector<SortColumn> &sortColumns) const {

        try {

            document query;
            mongocxx::options::find opts;
            if (!sortColumns.empty()) {
                document sort = {};
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                }
                opts.sort(sort.extract());
            }

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][GROUP_COLLECTION];
            std::vector<Entity::Cognito::Group> groups;
            for (auto groupCursor = col.find(query.extract(), opts); auto group: groupCursor) {
                Entity::Cognito::Group result;
                result.FromDocument(group);
                groups.push_back(result);
            }
            log_trace << "Got group list, size:" << groups.size();
            return groups;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long CognitoMongoRepository::deleteGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][GROUP_COLLECTION];
            const auto result = col.delete_many(make_document(kvp("region", region), kvp("userPoolId", userPoolId), kvp("groupName", groupName)));
            log_debug << "Group deleted, groupName: " << groupName << " count: " << result->deleted_count();
            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    long CognitoMongoRepository::deleteAllGroups(const std::string &region) const {

        try {

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][GROUP_COLLECTION];
            const auto result = col.delete_many(query.extract());
            log_debug << "All groups deleted, count: " << result->deleted_count();

            return result->deleted_count();

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    bool CognitoMongoRepository::clientIdExists(const std::string &region, const std::string &clientId) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            auto col = (*client)[DATABASE_NAME][GROUP_COLLECTION];
            const int64_t count = col.count_documents(make_document(kvp("region", region), kvp("userPoolClients", make_document(kvp("$elemMatch", make_document(kvp("clientId", clientId)))))));
            log_trace << "Cognito client id exists: " << std::boolalpha << count;
            return count > 0;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

}// namespace Awsmock::Database
