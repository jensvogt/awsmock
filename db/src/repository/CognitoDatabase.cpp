//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/CognitoDatabase.h>

namespace AwsMock::Database {

    bool CognitoDatabase::UserPoolExists(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];
                const int64_t count = _userPoolCollection.count_documents(make_document(kvp("region", region), kvp("name", name)));
                log_trace << "Cognito user pool exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UserPoolExists(region, name);
    }

    bool CognitoDatabase::UserPoolExists(const std::string &userPoolId) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];
                const int64_t count = _userPoolCollection.count_documents(make_document(kvp("userPoolId", userPoolId)));
                log_trace << "Cognito user pool exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UserPoolExists(userPoolId);
    }

    Entity::Cognito::UserPool CognitoDatabase::CreateUserPool(Entity::Cognito::UserPool &userPool) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _userPoolCollection.insert_one(userPool.ToDocument());
                session.commit_transaction();
                log_trace << "User pool created, oid: " << result->inserted_id().get_oid().value.to_string();
                userPool.oid = result->inserted_id().get_oid().value.to_string();
                return userPool;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CreateUserPool(userPool);
    }

    Entity::Cognito::UserPool CognitoDatabase::GetUserPoolById(bsoncxx::oid oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];
            const auto mResult = _userPoolCollection.find_one(make_document(kvp("_id", oid)));
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

    Entity::Cognito::UserPool CognitoDatabase::GetUserPoolByUserPoolId(const std::string &userPoolId) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];
                const auto mResult = _userPoolCollection.find_one(make_document(kvp("userPoolId", userPoolId)));
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
        return _memoryDb.GetUserPoolByUserPoolId(userPoolId);
    }

    Entity::Cognito::UserPool CognitoDatabase::GetUserPoolByClientId(const std::string &clientId) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];
                const auto mResult = _userPoolCollection.find_one(make_document(kvp("userPoolClients", make_document(kvp("$elemMatch", make_document(kvp("clientId", clientId)))))));
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
        return _memoryDb.GetUserPoolByClientId(clientId);
    }

    Entity::Cognito::UserPool CognitoDatabase::GetUserPoolByRegionName(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];
                const auto mResult = _userPoolCollection.find_one(make_document(kvp("region", region), kvp("name", name)));
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
        return _memoryDb.GetUserPoolByRegionName(region, name);
    }

    Entity::Cognito::UserPool CognitoDatabase::CreateOrUpdateUserPool(Entity::Cognito::UserPool &userPool) const {

        if (UserPoolExists(userPool.region, userPool.name)) {

            return UpdateUserPool(userPool);
        }
        return CreateUserPool(userPool);
    }

    Entity::Cognito::UserPool CognitoDatabase::UpdateUserPool(Entity::Cognito::UserPool &userPool) const {

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto mResult = _userPoolCollection.find_one_and_update(make_document(kvp("region", userPool.region), kvp("name", userPool.name)), userPool.ToDocument(), opts);
                session.commit_transaction();

                if (mResult) {
                    log_trace << "Cognito user pool updated: " << userPool.ToString();
                    userPool.FromDocument(mResult->view());
                    return userPool;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UpdateUserPool(userPool);
    }

    std::vector<Entity::Cognito::UserPool> CognitoDatabase::ListUserPools(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        std::vector<Entity::Cognito::UserPool> userPools;
        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];

                mongocxx::options::find opts;
                if (!sortColumns.empty()) {
                    document sort = {};
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
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

                for (auto userPoolCursor = _userPoolCollection.find(query.extract(), opts); const auto &userPool: userPoolCursor) {
                    Entity::Cognito::UserPool result;
                    result.FromDocument(userPool);
                    userPools.push_back(result);
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            userPools = _memoryDb.ListUserPools(region);
        }

        log_trace << "Got user pool list, size:" << userPools.size();
        return userPools;
    }

    std::vector<Entity::Cognito::UserPool> CognitoDatabase::ExportUserPools(const std::vector<SortColumn> &sortColumns) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];

                mongocxx::options::find opts;
                if (!sortColumns.empty()) {
                    document sort = {};
                    for (const auto [column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
                    }
                    opts.sort(sort.extract());
                }

                std::vector<Entity::Cognito::UserPool> userPools;
                for (auto userPoolCursor = _userPoolCollection.find({}, opts); auto userPool: userPoolCursor) {
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
        return _memoryDb.ExportUserPools(sortColumns);
    }

    long CognitoDatabase::CountUserPools(const std::string &region) const {

        if (HasDatabase()) {

            try {
                long count = 0;
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                count = _userPoolCollection.count_documents(query.extract());
                log_trace << "User pool count: " << count;
                return count;

            } catch (mongocxx::exception::system_error &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CountUserPools(region);
    }

    void CognitoDatabase::DeleteUserPool(const std::string &id) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _userPoolCollection.delete_many(make_document(kvp("userPoolId", id)));
                session.commit_transaction();
                log_debug << "User pool deleted, userPoolId: " << id << " count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteUserPool(id);
        }
    }

    void CognitoDatabase::DeleteAllUserPools() const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _userPoolCollection = (*client)[_databaseName][_userpoolCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _userPoolCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All cognito user pools deleted, count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteAllUserPools();
        }
    }

    bool CognitoDatabase::UserExists(const std::string &region, const std::string &userPoolId, const std::string &userName) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];
                const int64_t count = _userCollection.count_documents(make_document(kvp("region", region), kvp("userPoolId", userPoolId), kvp("userName", userName)));
                log_trace << "Cognito user exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UserExists(region, userPoolId, userName);
    }

    bool CognitoDatabase::UserExists(const std::string &region, const std::string &userName) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];
                const int64_t count = _userCollection.count_documents(make_document(kvp("region", region), kvp("userName", userName)));
                log_trace << "Cognito user exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UserExists(region, userName);
    }

    Entity::Cognito::User CognitoDatabase::CreateUser(Entity::Cognito::User &user) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _userCollection.insert_one(user.ToDocument());
                session.commit_transaction();
                log_trace << "User created, oid: " << result->inserted_id().get_oid().value.to_string();
                user.oid = result->inserted_id().get_oid().value.to_string();
                return user;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CreateUser(user);
    }

    Entity::Cognito::User CognitoDatabase::GetUserById(bsoncxx::oid oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];
            const auto mResult = _userCollection.find_one(make_document(kvp("_id", oid)));
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

    Entity::Cognito::User CognitoDatabase::GetUserByUserName(const std::string &region, const std::string &userPoolId, const std::string &userName) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];
                const auto mResult = _userCollection.find_one(make_document(kvp("region", region), kvp("userPoolId", userPoolId), kvp("userName", userName)));
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
        return _memoryDb.GetUserByUserName(region, userPoolId, userName);
    }

    Entity::Cognito::User CognitoDatabase::GetUserById(const std::string &oid) const {

        if (HasDatabase()) {

            return GetUserById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetUserByOid(oid);
    }

    long CognitoDatabase::CountUsers(const std::string &region, const std::string &userPoolId, const std::string &groupName) const {

        if (HasDatabase()) {

            try {

                long count = 0;
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];

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

                count = _userCollection.count_documents(query.extract());

                log_trace << "User count: " << count;
                return count;
            } catch (mongocxx::exception::system_error &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CountUsers(region);
    }

    std::vector<Entity::Cognito::User> CognitoDatabase::ListUsers(const std::string &region, const std::string &userPoolId, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        if (HasDatabase()) {

            try {
                std::vector<Entity::Cognito::User> users;

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];

                mongocxx::options::find opts;
                if (!sortColumns.empty()) {
                    document sort = {};
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
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

                for (auto userCursor = _userCollection.find(query.extract(), opts); const auto &user: userCursor) {
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
        return _memoryDb.ListUsers(region, userPoolId);
    }

    std::vector<Entity::Cognito::User> CognitoDatabase::ExportUsers(const std::vector<SortColumn> &sortColumns) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];
                std::vector<Entity::Cognito::User> users;

                document query;
                mongocxx::options::find opts;
                if (!sortColumns.empty()) {
                    document sort = {};
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
                    }
                    opts.sort(sort.extract());
                }

                for (auto userCursor = _userCollection.find(query.extract(), opts); auto user: userCursor) {
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
        return _memoryDb.ExportUsers(sortColumns);
    }

    std::vector<Entity::Cognito::User> CognitoDatabase::ListUsersInGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) const {

        std::vector<Entity::Cognito::User> users;
        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];

                auto userCursor = _userCollection.find(make_document(kvp("region", region), kvp("userPoolId", userPoolId),
                                                                     kvp("groups",
                                                                         make_document(kvp("$elemMatch", make_document(kvp("groupName", groupName)))))));
                for (auto user: userCursor) {
                    Entity::Cognito::User result;
                    result.FromDocument(user);
                    users.push_back(result);
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            users = _memoryDb.ListUsersInGroup(region, userPoolId, groupName);
        }

        log_trace << "Got users in group, size:" << users.size();
        return users;
    }

    Entity::Cognito::User CognitoDatabase::UpdateUser(Entity::Cognito::User &user) const {

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto mResult = _userCollection.find_one_and_update(make_document(kvp("region", user.region), kvp("userPoolId", user.userPoolId), kvp("userName", user.userName)), user.ToDocument(), opts);
                session.commit_transaction();

                if (mResult) {
                    log_trace << "Cognito user updated: " << user.ToString();
                    user.FromDocument(mResult->view());
                    return user;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UpdateUser(user);
    }

    Entity::Cognito::User CognitoDatabase::CreateOrUpdateUser(Entity::Cognito::User &user) const {

        if (UserExists(user.region, user.userPoolId, user.userName)) {

            return UpdateUser(user);
        }
        return CreateUser(user);
    }

    void CognitoDatabase::DeleteUser(const Entity::Cognito::User &user) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _userCollection.delete_many(make_document(kvp("region", user.region), kvp("userPoolId", user.userPoolId), kvp("userName", user.userName)));
                session.commit_transaction();
                log_debug << "User deleted, userName: " << user.userName << " count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteUser(user);
        }
    }

    void CognitoDatabase::DeleteAllUsers() const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _userCollection = (*client)[_databaseName][_userCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _userCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All cognito users deleted, count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteAllUsers();
        }
    }

    bool CognitoDatabase::GroupExists(const std::string &region, const std::string &groupName) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _groupCollection = (*client)[_databaseName][_groupCollectionName];
                const int64_t count = _groupCollection.count_documents(make_document(kvp("region", region), kvp("groupName", groupName)));
                log_trace << "Cognito group exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.GroupExists(region, groupName);
    }

    Entity::Cognito::Group CognitoDatabase::GetGroupById(bsoncxx::oid oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _groupCollection = (*client)[_databaseName][_groupCollectionName];
            const auto mResult = _groupCollection.find_one(make_document(kvp("_id", oid)));
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

    Entity::Cognito::Group CognitoDatabase::GetGroupByGroupName(const std::string &region, const std::string &userPoolId, const std::string &groupName) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _groupCollection = (*client)[_databaseName][_groupCollectionName];
                const auto mResult = _groupCollection.find_one(make_document(kvp("region", region), kvp("userPoolId", userPoolId), kvp("groupName", groupName)));
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
        return _memoryDb.GetGroupByGroupName(region, userPoolId, groupName);
    }

    // TODO: write tests
    Entity::Cognito::Group CognitoDatabase::CreateGroup(Entity::Cognito::Group &group) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _groupCollection = (*client)[_databaseName][_groupCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _groupCollection.insert_one(group.ToDocument());
                session.commit_transaction();
                log_trace << "Cognito group created, oid: " << result->inserted_id().get_oid().value.to_string();
                group.oid = result->inserted_id().get_oid().value.to_string();
                return group;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CreateGroup(group);
    }

    std::vector<Entity::Cognito::Group> CognitoDatabase::ListGroups(const std::string &region, const std::string &userPoolId) const {

        std::vector<Entity::Cognito::Group> groups;
        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _groupCollection = (*client)[_databaseName][_groupCollectionName];

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!userPoolId.empty()) {
                    query.append(kvp("userPoolId", userPoolId));
                }

                for (auto groupCursor = _groupCollection.find(query.extract()); auto group: groupCursor) {
                    Entity::Cognito::Group result;
                    result.FromDocument(group);
                    groups.push_back(result);
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            groups = _memoryDb.ListGroups(region, userPoolId);
        }

        log_trace << "Got group list, size:" << groups.size();
        return groups;
    }

    std::vector<Entity::Cognito::Group> CognitoDatabase::ExportGroups(const std::vector<SortColumn> &sortColumns) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _groupCollection = (*client)[_databaseName][_groupCollectionName];

                document query;
                mongocxx::options::find opts;
                if (!sortColumns.empty()) {
                    document sort = {};
                    for (const auto sortColumn: sortColumns) {
                        sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                    }
                    opts.sort(sort.extract());
                }

                std::vector<Entity::Cognito::Group> groups;
                for (auto groupCursor = _groupCollection.find(query.extract(), opts); auto group: groupCursor) {
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
        return _memoryDb.ExportGroups(sortColumns);
    }

    void CognitoDatabase::DeleteGroup(const std::string &region, const std::string &userPoolId, const std::string &groupName) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _groupCollection = (*client)[_databaseName][_groupCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _groupCollection.delete_many(make_document(kvp("region", region), kvp("userPoolId", userPoolId), kvp("groupName", groupName)));
                session.commit_transaction();
                log_debug << "Group deleted, groupName: " << groupName << " count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteGroup(region, userPoolId, groupName);
        }
    }

    void CognitoDatabase::DeleteAllGroups(const std::string &region) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _groupCollection = (*client)[_databaseName][_groupCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();

                document query;
                if (region.empty()) {
                    query.append(kvp("region", region));
                }
                const auto result = _groupCollection.delete_many(query.extract());
                log_debug << "All groups deleted, count: " << result->deleted_count();

                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteAllGroups(region);
        }
    }

    bool CognitoDatabase::ClientIdExists(const std::string &region, const std::string &clientId) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _groupCollection = (*client)[_databaseName][_userpoolCollectionName];
                const int64_t count = _groupCollection.count_documents(make_document(kvp("region", region),
                                                                                     kvp("userPoolClients",
                                                                                         make_document(kvp("$elemMatch",
                                                                                                           make_document(kvp("clientId", clientId)))))));
                log_trace << "Cognito client id exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ClientIdExists(region, clientId);
    }

}// namespace AwsMock::Database
