//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/DynamoDbDatabase.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    DynamoDbDatabase::DynamoDbDatabase() : _databaseName(GetDatabaseName()), _tableCollectionName("dynamodb_table"), _itemCollectionName("dynamodb_item"), _memoryDb(DynamoDbMemoryDb::instance()) {}

    Entity::DynamoDb::Table DynamoDbDatabase::CreateTable(const Entity::DynamoDb::Table &table) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _tableCollection.insert_one(table.ToDocument());
                session.commit_transaction();
                log_trace << "DynamoDb table created, oid: " << result->inserted_id().get_oid().value.to_string();
                return GetTableById(result->inserted_id().get_oid().value);

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CreateTable(table);
    }

    Entity::DynamoDb::Table DynamoDbDatabase::GetTableById(bsoncxx::oid oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
            const std::optional<value> mResult = _tableCollection.find_one(make_document(kvp("_id", oid)));
            if (!mResult) {
                log_error << "Database exception: Table not found ";
                throw Core::DatabaseException("Database exception, Table not found ");
            }

            Entity::DynamoDb::Table result;
            result.FromDocument(mResult->view());
            log_debug << "Got table by ID, table: " << result.ToString();
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::DynamoDb::Table DynamoDbDatabase::GetTableByRegionName(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
                const std::optional<value> mResult = _tableCollection.find_one(make_document(kvp("region", region), kvp("name", name)));
                if (!mResult) {
                    log_error << "Database exception: Table not found ";
                    throw Core::DatabaseException("Database exception, Table not found ");
                }

                Entity::DynamoDb::Table result;
                result.FromDocument(mResult->view());
                log_debug << "Got table by ID, table: " << result.ToString();
                return result;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.GetTableByRegionName(region, name);
    }

    Entity::DynamoDb::Table DynamoDbDatabase::GetTableById(const std::string &oid) const {

        if (HasDatabase()) {

            return GetTableById(bsoncxx::oid(oid));
        }
        return _memoryDb.GetTableById(oid);
    }

    bool DynamoDbDatabase::TableExists(const std::string &region, const std::string &tableName) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!tableName.empty()) {
                    query.append(kvp("name", tableName));
                }

                const int64_t count = _tableCollection.count_documents(query.extract());
                log_trace << "DynamoDb table exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.TableExists(region, tableName);
    }

    std::vector<Entity::DynamoDb::Table> DynamoDbDatabase::ListTables(const std::string &region) const {

        if (HasDatabase()) {

            try {
                Entity::DynamoDb::TableList tables;
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];

                document query = {};
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }

                for (auto tableCursor = _tableCollection.find(query.extract()); auto table: tableCursor) {
                    Entity::DynamoDb::Table result;
                    result.FromDocument(table);
                    tables.push_back(result);
                }

                log_trace << "Got DynamoDb table list, size:" << tables.size();
                return tables;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ListTables(region);
    }

    long DynamoDbDatabase::CountTables(const std::string &region) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];

                document query = {};
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                return _tableCollection.count_documents(query.extract());

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CountTables(region);
    }

    Entity::DynamoDb::Table DynamoDbDatabase::CreateOrUpdateTable(Entity::DynamoDb::Table &table) const {

        if (TableExists(table.region, table.name)) {

            return UpdateTable(table);
        }
        return CreateTable(table);
    }

    Entity::DynamoDb::Table DynamoDbDatabase::UpdateTable(Entity::DynamoDb::Table &table) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
            auto session = client->start_session();

            try {

                mongocxx::options::find_one_and_update opts{};
                opts.return_document(mongocxx::options::return_document::k_after);

                document query = {};
                query.append(kvp("region", table.region));
                query.append(kvp("name", table.name));

                session.start_transaction();
                const auto mResult = _tableCollection.find_one_and_update(query.extract(), table.ToDocument(), opts);
                session.commit_transaction();

                if (mResult) {
                    table.FromDocument(mResult->view());
                    log_trace << "DynamoDB table updated: " << table.name;
                    return table;
                }
                return {};

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UpdateTable(table);
    }

    void DynamoDbDatabase::DeleteTable(const std::string &region, const std::string &tableName) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                auto result = _tableCollection.delete_many(make_document(kvp("region", region), kvp("name", tableName)));
                session.commit_transaction();
                log_debug << "DynamoDB table deleted, tableName: " << tableName << " region: " << region;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteTable(tableName);
        }
    }

    void DynamoDbDatabase::DeleteAllTables() const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _tableCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All DynamoDb tables deleted, count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteAllTables();
        }
    }

    /*bool DynamoDbDatabase::ItemExists(const std::string &region, const std::string &tableName) {

        if (HasDatabase()) {

            try {

                int64_t count;
                auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
                if (!region.empty()) {
                    count = _tableCollection.count_documents(make_document(kvp("region", region), kvp("name", tableName)));
                } else {
                    count = _tableCollection.count_documents(make_document(kvp("name", tableName)));
                }
                log_trace << "DynamoDb table exists: " << std::boolalpha<<(count>0);
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            return _memoryDb.ItemExists(region, tableName, key);
        }
    }*/

    bool DynamoDbDatabase::ItemExists(const Entity::DynamoDb::Item &item) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];

                Entity::DynamoDb::Table table = GetTableByRegionName(item.region, item.tableName);

                document query;
                if (!item.region.empty()) {
                    query.append(kvp("region", item.region));
                }

                if (!item.tableName.empty()) {
                    query.append(kvp("name", item.tableName));
                }

                // Add primary keys
                for (const auto &key: table.keySchemas | std::views::keys) {
                    std::string keyName = key;
                    std::map<std::string, Entity::DynamoDb::AttributeValue> att = item.attributes;
                    auto it = std::ranges::find_if(att,
                                                   [keyName](const std::pair<std::string, Entity::DynamoDb::AttributeValue> &attribute) {
                                                       return attribute.first == keyName;
                                                   });
                    if (it != att.end()) {
                        query.append(kvp("attributes." + key + ".S", it->second.stringValue));
                        query.append(kvp("attributes." + key + ".N", it->second.numberValue));
                        query.append(kvp("attributes." + key + ".BOOL", it->second.boolValue));
                        query.append(kvp("attributes." + key + ".NULL", it->second.boolValue));
                    }
                }
                const int64_t count = _itemCollection.count_documents(query.extract());

                log_trace << "DynamoDb table exists: " << std::boolalpha << count;
                return count > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ItemExists(item);
    }

    Entity::DynamoDb::ItemList DynamoDbDatabase::ListItems(const std::string &region, const std::string &tableName) const {

        Entity::DynamoDb::ItemList items;
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName]["dynamodb_item"];
            try {

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }

                if (!tableName.empty()) {
                    query.append(kvp("name", tableName));
                }

                for (auto itemCursor = _itemCollection.find(query.extract()); auto item: itemCursor) {
                    Entity::DynamoDb::Item result;
                    result.FromDocument(item);
                    items.push_back(result);
                }

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            items = _memoryDb.ListItems(region, tableName);
        }

        log_trace << "Got DynamoDb item list, size:" << items.size();
        return items;
    }

    Entity::DynamoDb::Item DynamoDbDatabase::GetItemById(bsoncxx::oid oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            const std::optional<value> mResult = _itemCollection.find_one(make_document(kvp("_id", oid)));
            if (!mResult) {
                log_error << "Database exception: item not found ";
                throw Core::DatabaseException("Database exception, item not found ");
            }

            Entity::DynamoDb::Item result;
            result.FromDocument(mResult->view());
            log_debug << "Got item by ID, item: " << result.ToString();
            return result;

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
    }

    Entity::DynamoDb::Item DynamoDbDatabase::CreateItem(const Entity::DynamoDb::Item &item) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                log_info << to_json(item.ToDocument());
                const auto result = _itemCollection.insert_one(item.ToDocument());
                session.commit_transaction();
                log_trace << "DynamoDb item created, oid: " << result->inserted_id().get_oid().value.to_string();
                return GetItemById(result->inserted_id().get_oid().value);

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CreateItem(item);
    }

    Entity::DynamoDb::Item DynamoDbDatabase::UpdateItem(const Entity::DynamoDb::Item &item) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            auto session = client->start_session();

            try {

                Entity::DynamoDb::Table table = GetTableByRegionName(item.region, item.tableName);

                document query;
                if (!item.region.empty()) {
                    query.append(kvp("region", item.region));
                }

                if (!item.tableName.empty()) {
                    query.append(kvp("name", item.tableName));
                }

                // Add primary keys
                for (const auto &key: table.keySchemas | std::views::keys) {
                    const std::string &keyName = key;
                    std::map<std::string, Entity::DynamoDb::AttributeValue> att = item.attributes;
                    auto it = std::ranges::find_if(att,
                                                   [keyName](const std::pair<std::string, Entity::DynamoDb::AttributeValue> &attribute) {
                                                       return attribute.first == keyName;
                                                   });
                    if (it != att.end()) {
                        query.append(kvp("attributes." + key + ".S", it->second.stringValue));
                        query.append(kvp("attributes." + key + ".N", it->second.numberValue));
                        query.append(kvp("attributes." + key + ".BOOL", it->second.boolValue));
                        query.append(kvp("attributes." + key + ".NULL", it->second.boolValue));
                    }
                }

                session.start_transaction();
                auto result = _itemCollection.replace_one(query.extract(), item.ToDocument());
                const auto findResult = _itemCollection.find_one(query.extract());
                session.commit_transaction();

                log_info << "DynamoDb item updated, oid: " << findResult.value()["_id"].get_oid().value.to_string();
                return Entity::DynamoDb::Item().FromDocument(findResult->view());

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UpdateItem(item);
    }

    Entity::DynamoDb::Item DynamoDbDatabase::CreateOrUpdateItem(const Entity::DynamoDb::Item &item) const {

        if (ItemExists(item)) {
            return UpdateItem(item);
        }
        return CreateItem(item);
    }

    long DynamoDbDatabase::CountItems(const std::string &region) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
                if (region.empty()) {

                    return _itemCollection.count_documents({});
                }
                return _itemCollection.count_documents(make_document(kvp("region", region)));

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CountItems(region);
    }

    void DynamoDbDatabase::DeleteItem(const std::string &region, const std::string &tableName, const std::string &key) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
                const auto result = _itemCollection.delete_many(make_document(kvp("name", tableName)));
                log_debug << "DynamoDB item deleted, tableName: " << tableName << " count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteItem(region, tableName, key);
        }
    }

    void DynamoDbDatabase::DeleteItems(const std::string &region, const std::string &tableName) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
                const auto result = _itemCollection.delete_many(make_document(kvp("tableName", tableName)));
                log_debug << "DynamoDB item deleted, tableName: " << tableName << " count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteItems(region, tableName);
        }
    }

    void DynamoDbDatabase::DeleteAllItems() const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _itemCollection = (*client)[_databaseName]["dynamodb_item"];
                const auto result = _itemCollection.delete_many({});
                log_debug << "DynamoDB items deleted, count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteAllItems();
        }
    }

}// namespace AwsMock::Database
