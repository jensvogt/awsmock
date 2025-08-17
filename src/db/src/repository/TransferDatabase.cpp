//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/TransferDatabase.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    TransferDatabase::TransferDatabase() : _memoryDb(TransferMemoryDb::instance()), _databaseName(GetDatabaseName()), _serverCollectionName("transfer") {}

    bool TransferDatabase::TransferExists(const std::string &region, const std::string &serverId) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
            const int64_t count = _transferCollection.count_documents(make_document(kvp("region", region), kvp("serverId", serverId)));
            log_trace << "Transfer server exists: " << std::boolalpha << count;
            return count > 0;
        }
        return _memoryDb.TransferExists(region, serverId);
    }

    bool TransferDatabase::TransferExists(const Entity::Transfer::Transfer &transfer) const {

        return TransferExists(transfer.region, transfer.serverId);
    }

    bool TransferDatabase::TransferExists(const std::string &serverId) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
            const int64_t count = _transferCollection.count_documents(make_document(kvp("serverId", serverId)));
            log_trace << "Transfer server exists: " << std::boolalpha << count;
            return count > 0;
        }
        return _memoryDb.TransferExists(serverId);
    }

    bool TransferDatabase::TransferExists(const std::string &region, const std::vector<Entity::Transfer::Protocol> &protocols) const {

        if (HasDatabase()) {

            array mProtocol{};
            for (const auto &p: protocols) {
                mProtocol.append(p);
            }

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
            const int64_t count = _transferCollection.count_documents(make_document(kvp("region", region), kvp("protocols", make_document(kvp("$elemMatch", make_document(kvp("$in", mProtocol)))))));
            log_trace << "Transfer server exists: " << std::boolalpha << count;
            return count > 0;
        }
        return _memoryDb.TransferExists(region, protocols);
    }

    Entity::Transfer::Transfer TransferDatabase::CreateTransfer(const Entity::Transfer::Transfer &transfer) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
            const auto result = _transferCollection.insert_one(transfer.ToDocument());
            log_trace << "Transfer server created, oid: " << result->inserted_id().get_oid().value.to_string();

            return GetTransferById(result->inserted_id().get_oid().value);
        }
        return _memoryDb.CreateTransfer(transfer);
    }

    Entity::Transfer::Transfer TransferDatabase::GetTransferById(bsoncxx::oid oid) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
        const auto mResult = _transferCollection.find_one(make_document(kvp("_id", oid)));
        Entity::Transfer::Transfer result;
        result.FromDocument(mResult->view());
        return result;
    }

    Entity::Transfer::Transfer TransferDatabase::GetTransferById(const std::string &oid) const {

        if (HasDatabase()) {

            return GetTransferById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetTransferById(oid);
    }

    Entity::Transfer::Transfer TransferDatabase::GetTransferByServerId(const std::string &region, const std::string &serverId) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!serverId.empty()) {
                query.append(kvp("serverId", serverId));
            }

            if (const auto mResult = _transferCollection.find_one(query.extract()); mResult) {
                Entity::Transfer::Transfer result;
                result.FromDocument(mResult->view());
                return result;
            }
            return {};
        }
        return _memoryDb.GetTransferByServerId(region, serverId);
    }

    Entity::Transfer::Transfer TransferDatabase::CreateOrUpdateTransfer(const Entity::Transfer::Transfer &transfer) const {

        if (TransferExists(transfer)) {
            return UpdateTransfer(transfer);
        }
        return CreateTransfer(transfer);
    }

    Entity::Transfer::Transfer TransferDatabase::UpdateTransfer(const Entity::Transfer::Transfer &transfer) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
            auto result = _transferCollection.find_one_and_update(make_document(kvp("region", transfer.region), kvp("serverId", transfer.serverId)), transfer.ToDocument());
            log_trace << "Transfer updated: " << transfer.ToString();
            return GetTransferByServerId(transfer.region, transfer.serverId);
        }
        return _memoryDb.UpdateTransfer(transfer);
    }

    Entity::Transfer::Transfer TransferDatabase::GetTransferByArn(const std::string &arn) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
            const auto mResult = _transferCollection.find_one(make_document(kvp("arn", arn)));
            Entity::Transfer::Transfer result;
            result.FromDocument(mResult->view());
            return result;
        }
        return _memoryDb.GetTransferByArn(arn);
    }

    std::vector<Entity::Transfer::Transfer> TransferDatabase::ListServers(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        std::vector<Entity::Transfer::Transfer> transfers;

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!prefix.empty()) {
                    query.append(kvp("serverId", make_document(kvp("$regex", "^" + prefix))));
                }

                mongocxx::options::find opts;
                if (!sortColumns.empty()) {
                    document sort = {};
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
                    }
                    opts.sort(sort.extract());
                }
                if (pageIndex > 0) {
                    opts.skip(pageIndex * pageSize);
                }
                if (pageSize > 0) {
                    opts.limit(pageSize);
                }

                for (auto transferCursor = _transferCollection.find(query.extract(), opts); auto transfer: transferCursor) {
                    Entity::Transfer::Transfer result;
                    result.FromDocument(transfer);
                    transfers.push_back(result);
                }

                log_trace << "Got transfer list, size:" << transfers.size();

            } catch (mongocxx::exception::system_error &e) {
                log_error << "List servers failed, error: " << e.what();
            }

        } else {

            return _memoryDb.ListServers(region);
        }
        return transfers;
    }

    std::vector<Entity::Transfer::Transfer> TransferDatabase::ListServers(const std::string &region, std::string &nextToken, const long maxResults) const {

        std::vector<Entity::Transfer::Transfer> transfers;

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!nextToken.empty()) {
                    query.append(kvp("serverId", make_document(kvp("$gte", nextToken))));
                }

                mongocxx::options::find opts;
                document sort = {};
                sort.append(kvp("serverId", 1));
                opts.sort(sort.extract());

                if (maxResults > 0) {
                    opts.limit(maxResults + 1);
                }

                for (auto transferCursor = _transferCollection.find(query.extract(), opts); const auto transfer: transferCursor) {
                    Entity::Transfer::Transfer result;
                    result.FromDocument(transfer);
                    transfers.push_back(result);
                    nextToken = result.serverId;
                }
                if (transfers.size() < maxResults) {
                    nextToken = {};
                } else if (!transfers.empty()) {
                    transfers.pop_back();
                }
                log_trace << "Got transfer list, size:" << transfers.size();

            } catch (mongocxx::exception::system_error &e) {
                log_error << "List servers failed, error: " << e.what();
            }

        } else {

            transfers = _memoryDb.ListServers(region);
        }
        return transfers;
    }

    std::vector<Entity::Transfer::User> TransferDatabase::ListUsers(const std::string &region, const std::string &serverId, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        std::vector<Entity::Transfer::Transfer> transfers;

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];

                mongocxx::options::find opts;
                if (pageSize > 0) {
                    opts.limit(pageSize);
                }
                if (pageIndex > 0) {
                    opts.skip(pageIndex * pageSize);
                }

                opts.sort(make_document(kvp("_id", 1)));
                if (!sortColumns.empty()) {
                    document sort;
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
                    }
                    opts.sort(sort.extract());
                }

                document query = {};
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!serverId.empty()) {
                    query.append(kvp("serverId", serverId));
                }
                if (!prefix.empty()) {
                    query.append(kvp("serverId", make_document(kvp("$regex", "^" + prefix))));
                } else if (!serverId.empty()) {
                    query.append(kvp("serverId", serverId));
                }

                if (const auto mResult = _transferCollection.find_one(query.extract()); mResult.has_value()) {
                    Entity::Transfer::Transfer result;
                    result.FromDocument(mResult->view());
                    log_trace << "Got transfer server, serverId:" << serverId;
                    return result.users;
                }

            } catch (mongocxx::exception::system_error &e) {
                log_error << "List servers failed, error: " << e.what();
            }

        } else {

            return _memoryDb.ListUsers(region, serverId);
        }
        return {};
    }

    long TransferDatabase::CountServers(const std::string &region) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _serverCollection = (*client)[_databaseName][_serverCollectionName];

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }

            const long count = _serverCollection.count_documents(query.extract());
            log_trace << "Count servers, result: " << count;
            return count;
        }
        return _memoryDb.CountServers(region);
    }

    long TransferDatabase::CountUsers(const std::string &region, const std::string &serverId) const {

        if (HasDatabase()) {

            const Entity::Transfer::Transfer transfer = GetTransferByServerId(region, serverId);
            log_trace << "Count users, count: " << transfer.users.size();
            return transfer.users.size();
        }
        return 0;
    }

    void TransferDatabase::DeleteTransfer(const std::string &serverId) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
            const auto result = _transferCollection.delete_many(make_document(kvp("serverId", serverId)));
            log_debug << "Transfer deleted, serverId: " << serverId << " count: " << result->deleted_count();

        } else {

            _memoryDb.DeleteTransfer(serverId);
        }
    }

    long TransferDatabase::DeleteAllTransfers() const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
            const auto result = _transferCollection.delete_many({});
            log_debug << "All transfers deleted, count: " << result->deleted_count();
            return result->deleted_count();
        }
        return _memoryDb.DeleteAllTransfers();
    }

}// namespace AwsMock::Database
