//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/transfer/TransferMongoRepository.h>

namespace Awsmock::Database {

    bool TransferMongoRepository::transferExists(const std::string &region, const std::string &serverId) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
        const int64_t count = _transferCollection.count_documents(make_document(kvp("region", region), kvp("serverId", serverId)));
        log_trace << "Transfer server exists: " << std::boolalpha << count;
        return count > 0;
    }

    bool TransferMongoRepository::transferExists(const Entity::Transfer::Transfer &transfer) const {
        return transferExists(transfer.region, transfer.serverId);
    }

    bool TransferMongoRepository::transferExists(const std::string &serverId) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
        const int64_t count = _transferCollection.count_documents(make_document(kvp("serverId", serverId)));
        log_trace << "Transfer server exists: " << std::boolalpha << count;
        return count > 0;
    }

    bool TransferMongoRepository::transferExists(const std::string &region, const std::vector<Entity::Transfer::Protocol> &protocols) const {

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

    Entity::Transfer::Transfer TransferMongoRepository::createTransfer(const Entity::Transfer::Transfer &transfer) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
        const auto result = _transferCollection.insert_one(transfer.ToDocument());
        log_trace << "Transfer server created, oid: " << result->inserted_id().get_oid().value.to_string();

        return getTransferById(result->inserted_id().get_oid().value);
    }

    Entity::Transfer::Transfer TransferMongoRepository::getTransferById(const bsoncxx::oid &oid) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
        const auto mResult = _transferCollection.find_one(make_document(kvp("_id", oid)));
        Entity::Transfer::Transfer result;
        result.FromDocument(mResult->view());
        return result;
    }

    Entity::Transfer::Transfer TransferMongoRepository::getTransferById(const std::string &oid) const {
        return getTransferById(bsoncxx::oid(oid));
    }

    Entity::Transfer::Transfer TransferMongoRepository::getTransferByServerId(const std::string &region, const std::string &serverId) const {

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

    Entity::Transfer::Transfer TransferMongoRepository::createOrUpdateTransfer(const Entity::Transfer::Transfer &transfer) const {

        if (transferExists(transfer)) {
            return updateTransfer(transfer);
        }
        return createTransfer(transfer);
    }

    Entity::Transfer::Transfer TransferMongoRepository::updateTransfer(const Entity::Transfer::Transfer &transfer) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
        auto result = _transferCollection.find_one_and_update(make_document(kvp("region", transfer.region), kvp("serverId", transfer.serverId)), transfer.ToDocument());
        log_trace << "Transfer updated: " << transfer.ToString();
        return getTransferByServerId(transfer.region, transfer.serverId);
    }

    Entity::Transfer::Transfer TransferMongoRepository::getTransferByArn(const std::string &arn) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
        const auto mResult = _transferCollection.find_one(make_document(kvp("arn", arn)));
        Entity::Transfer::Transfer result;
        result.FromDocument(mResult->view());
        return result;
    }

    std::vector<Entity::Transfer::Transfer> TransferMongoRepository::listServers(const std::string &region, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {


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
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
                }
                opts.sort(sort.extract());
            }
            if (pageIndex > 0) {
                opts.skip(pageIndex * pageSize);
            }
            if (pageSize > 0) {
                opts.limit(pageSize);
            }

            std::vector<Entity::Transfer::Transfer> transfers;
            for (auto transferCursor = _transferCollection.find(query.extract(), opts); const auto transfer: transferCursor) {
                Entity::Transfer::Transfer result;
                result.FromDocument(transfer);
                transfers.push_back(result);
            }
            log_trace << "Got transfer list, size:" << transfers.size();
            return transfers;

        } catch (std::exception &e) {
            log_error << "List servers failed, error: " << e.what();
        }
        return {};
    }

    std::vector<Entity::Transfer::Transfer> TransferMongoRepository::listServers(const std::string &region, std::string &nextToken, const long maxResults) const {

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

            std::vector<Entity::Transfer::Transfer> transfers;
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

        } catch (std::exception &e) {
            log_error << "List servers failed, error: " << e.what();
        }
        return {};
    }

    std::vector<Entity::Transfer::User> TransferMongoRepository::listUsers(const std::string &region, const std::string &serverId, const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {

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
                for (const auto &sortColumn: sortColumns) {
                    sort.append(kvp(sortColumn.column, sortColumn.sortDirection));
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

            std::vector<Entity::Transfer::Transfer> transfers;
            if (const auto mResult = _transferCollection.find_one(query.extract()); mResult.has_value()) {
                Entity::Transfer::Transfer result;
                result.FromDocument(mResult->view());
                log_trace << "Got transfer server, serverId:" << serverId;
                return result.users;
            }

        } catch (std::exception &e) {
            log_error << "List servers failed, error: " << e.what();
        }
        return {};
    }

    long TransferMongoRepository::countServers(const std::string &region) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _serverCollection = (*client)[_databaseName][_serverCollectionName];

        document query;
        if (!region.empty()) {
            query.append(kvp("region", region));
        }

        const long count = _serverCollection.count_documents(query.view());
        log_trace << "Count servers, result: " << count;
        return count;
    }

    long TransferMongoRepository::countUsers(const std::string &region, const std::string &serverId) const {

        const Entity::Transfer::Transfer transfer = getTransferByServerId(region, serverId);
        log_trace << "Count users, count: " << transfer.users.size();
        return transfer.users.size();
    }

    void TransferMongoRepository::deleteTransfer(const std::string &serverId) const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
        const auto result = _transferCollection.delete_many(make_document(kvp("serverId", serverId)));
        log_debug << "Transfer deleted, serverId: " << serverId << " count: " << result->deleted_count();
    }

    long TransferMongoRepository::deleteAllTransfers() const {

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _transferCollection = (*client)[_databaseName][_serverCollectionName];
        const auto result = _transferCollection.delete_many({});
        log_debug << "All transfers deleted, count: " << result->deleted_count();
        return result->deleted_count();
    }

}// namespace Awsmock::Database
