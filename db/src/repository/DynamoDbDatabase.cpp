//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/DynamoDbDatabase.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;

    DynamoDbDatabase::DynamoDbDatabase() : _databaseName(GetDatabaseName()), _tableCollectionName("dynamodb_table"), _itemCollectionName("dynamodb_item"), _memoryDb(DynamoDbMemoryDb::instance()) {

        _accountId = Core::Configuration::instance().GetValue<std::string>("awsmock.access.account-id");
        _segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, MONITORING_SEGMENT_NAME);
        _dynamoDbCounterMap = _segment.find<DynamoDbCounterMapType>(DYNAMODB_COUNTER_MAP_NAME).first;
        if (!_dynamoDbCounterMap) {
            _dynamoDbCounterMap = _segment.construct<DynamoDbCounterMapType>(DYNAMODB_COUNTER_MAP_NAME)(std::less<std::string>(), _segment.get_segment_manager());
        }

        // Initialize the counters
        for (const auto &table: ListTables()) {
            DynamoDbMonitoringCounter counter;
            counter.items = CountItems(table.region, table.name);
            counter.size = GetTableSize(table.region, table.name);
            _dynamoDbCounterMap->insert_or_assign(table.arn, counter);
        }
        log_debug << "DynamoDb counters initialized" << _dynamoDbCounterMap->size();
    }

    Entity::DynamoDb::Table DynamoDbDatabase::CreateTable(Entity::DynamoDb::Table &table) const {

        table.created = table.modified = system_clock::now();

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _tableCollection.insert_one(table.ToDocument());
                session.commit_transaction();
                table.oid = result->inserted_id().get_oid().value.to_string();

                log_trace << "DynamoDb table created, oid: " << table.oid;

                return table;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CreateTable(table);
    }

    Entity::DynamoDb::Table DynamoDbDatabase::GetTableById(bsoncxx::oid oid) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
                const auto mResult = _tableCollection.find_one(make_document(kvp("_id", oid)));
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
        return {};
    }

    Entity::DynamoDb::Table DynamoDbDatabase::GetTableByRegionName(const std::string &region, const std::string &name) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
                const auto mResult = _tableCollection.find_one(make_document(kvp("region", region), kvp("name", name)));
                if (!mResult) {
                    log_error << "Database exception: Table not found ";
                    throw Core::DatabaseException("Database exception, Table not found ");
                }

                Entity::DynamoDb::Table result;
                result.FromDocument(mResult->view());
                log_trace << "Got table by ID, table: " << result.ToString();
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

                // Set limit to 1 (Very important for performance!)
                mongocxx::options::count options;
                options.limit(1);

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!tableName.empty()) {
                    query.append(kvp("name", tableName));
                }

                return _tableCollection.count_documents(query.view(), options) > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.TableExists(region, tableName);
    }

    std::vector<Entity::DynamoDb::Table> DynamoDbDatabase::ListTables(const std::string &region, const std::string &prefix, int pageSize, int pageIndex, const std::vector<SortColumn> &sortColumns) const {

        if (HasDatabase()) {

            try {

                mongocxx::options::find opts;

                Entity::DynamoDb::TableList tables;
                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];

                document query = {};
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!prefix.empty()) {
                    query.append(kvp("name", make_document(kvp("$regex", "^" + prefix))));
                }

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

                for (auto tableCursor = _tableCollection.find(query.extract(), opts); auto table: tableCursor) {
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

    long DynamoDbDatabase::CountTables(const std::string &region, const std::string &prefix) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];

                document query = {};
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!prefix.empty()) {
                    query.append(kvp("name", bsoncxx::types::b_regex{"^" + prefix + ".*"}));
                }
                return _tableCollection.count_documents(query.view());

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CountTables(region);
    }

    long DynamoDbDatabase::GetTableSize(const std::string &region, const std::string &tableName) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];

            try {
                mongocxx::pipeline p{};
                p.match(make_document(kvp("region", region), kvp("tableName", tableName)));
                p.group(make_document(kvp("_id", ""), kvp("totalSize", make_document(kvp("$sum", "$size")))));
                p.project(make_document(kvp("_id", 0), kvp("totalSize", "$totalSize")));
                auto totalSizeCursor = _itemCollection.aggregate(p);
                if (const auto t = *totalSizeCursor.begin(); !t.empty()) {
                    return t["totalSize"].get_int64().value;
                }
                return 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.GetTableSize(region);
    }

    Entity::DynamoDb::Table DynamoDbDatabase::CreateOrUpdateTable(Entity::DynamoDb::Table &table) const {

        if (TableExists(table.region, table.name)) {
            return UpdateTable(table);
        }
        return CreateTable(table);
    }

    Entity::DynamoDb::Table DynamoDbDatabase::UpdateTable(Entity::DynamoDb::Table &table) const {

        table.modified = system_clock::now();

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

    void DynamoDbDatabase::UpdateTableCounter(const std::string &tableArn, const long items, const long size) const {

        if (HasDatabase()) {

            mongocxx::options::find_one_and_update opts{};
            opts.return_document(mongocxx::options::return_document::k_after);

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _bucketCollection = (*client)[_databaseName][_tableCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();

                document filterQuery;
                filterQuery.append(kvp("arn", tableArn));

                document setQuery;
                setQuery.append(kvp("items", static_cast<bsoncxx::types::b_int64>(items)));
                setQuery.append(kvp("size", static_cast<bsoncxx::types::b_int64>(size)));

                document updateQuery;
                updateQuery.append(kvp("$set", setQuery));

                _bucketCollection.update_one(filterQuery.extract(), updateQuery.extract());
                log_trace << "Bucket counter updated";
                session.commit_transaction();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        _memoryDb.UpdateTableCounter(tableArn, items, size);
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
                return;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        _memoryDb.DeleteTable(tableName);
    }

    long DynamoDbDatabase::DeleteAllTables() const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto result = _tableCollection.delete_many({});
                session.commit_transaction();
                log_debug << "All DynamoDb tables deleted, count: " << result->deleted_count();
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.DeleteAllTables();
    }

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
                    query.append(kvp("tableName", item.tableName));
                }

                // Add primary keys
                for (const auto &v: table.keySchema) {
                    std::string keyName = v.attributeName;
                    std::map<std::string, Entity::DynamoDb::AttributeValue> att = item.attributes;
                    auto it = std::ranges::find_if(att,
                                                   [keyName](const std::pair<std::string, Entity::DynamoDb::AttributeValue> &attribute) {
                                                       return attribute.first == keyName;
                                                   });
                    if (it != att.end()) {
                        if (!it->second.stringValue.empty()) {
                            query.append(kvp("attributes." + keyName + ".S", it->second.stringValue));
                        }
                        if (!it->second.numberValue.empty()) {
                            query.append(kvp("attributes." + keyName + ".N", it->second.numberValue));
                        }
                        if (it->second.boolValue) {
                            query.append(kvp("attributes." + keyName + ".BOOL", *it->second.boolValue));
                        }
                        if (it->second.nullValue && *it->second.nullValue) {
                            query.append(kvp("attributes." + keyName + ".nullptr", *it->second.nullValue));
                        }
                    }
                }
                const int64_t count = _itemCollection.count_documents(query.view());

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

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName]["dynamodb_item"];
            try {
                Entity::DynamoDb::ItemList items;

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }

                if (!tableName.empty()) {
                    query.append(kvp("tableName", tableName));
                }

                for (auto itemCursor = _itemCollection.find(query.extract()); const auto item: itemCursor) {
                    Entity::DynamoDb::Item result;
                    result.FromDocument(item);
                    items.push_back(result);
                }
                log_trace << "Got DynamoDb item list, size:" << items.size();
                return items;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ListItems(region, tableName);
    }

    Entity::DynamoDb::Item DynamoDbDatabase::GetItemById(bsoncxx::oid oid) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            const auto mResult = _itemCollection.find_one(make_document(kvp("_id", oid)));
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

    Entity::DynamoDb::Item DynamoDbDatabase::GetItemByKeys(const std::string &region, const std::string &tableName, const std::map<std::string, Entity::DynamoDb::AttributeValue> &keys) const {

        try {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];

            document query;
            if (!region.empty()) {
                query.append(kvp("region", region));
            }
            if (!tableName.empty()) {
                query.append(kvp("tableName", tableName));
            }

            // Primary keys
            if (!keys.empty()) {
                document keyObject;
                for (const auto &[k, v]: keys) {
                    keyObject.append(kvp(k, v.ToDocument()));
                }
                query.append(kvp("keys", keyObject));
            }

            if (const auto mResult = _itemCollection.find_one(query.view())) {
                Entity::DynamoDb::Item result;
                result.FromDocument(mResult->view());
                log_debug << "Got item by ID, item: " << result.ToString();
                return result;
            }
            log_error << "Database exception: item not found, region: " << region << ", tableName: " << tableName << ", ,query: " << bsoncxx::to_json(query);
            throw Core::DatabaseException("Database exception, item not found, region: " + region + ", tableName: " + tableName);

        } catch (const mongocxx::exception &exc) {
            log_error << "Database exception " << exc.what();
            throw Core::DatabaseException("Database exception " + std::string(exc.what()));
        }
        return _memoryDb.GetItemByKeys(region, tableName, keys);
    }

    Entity::DynamoDb::Item DynamoDbDatabase::CreateItem(Entity::DynamoDb::Item &item) const {

        item.created = system_clock::now();
        item.size = sizeof(item) + sizeof(long);

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
            auto session = client->start_session();

            try {

                document tableSearchQuery;
                tableSearchQuery.append(kvp("tableName", item.tableName));

                document tableUpdateQuery;
                tableUpdateQuery.append(kvp("$inc", make_document(kvp("itemCount", 1))));
                tableUpdateQuery.append(kvp("$inc", make_document(kvp("size", item.size))));

                session.start_transaction();
                const auto itemResult = _itemCollection.insert_one(item.ToDocument());
                _tableCollection.update_one(tableSearchQuery.view(), tableUpdateQuery.view());
                session.commit_transaction();

                log_trace << "DynamoDb item created, oid: " << itemResult->inserted_id().get_oid().value.to_string();
                item.oid = itemResult->inserted_id().get_oid().value.to_string();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        } else {
            item = _memoryDb.CreateItem(item);
        }

        Core::AwsUtils::CreateDynamoDbTableArn(_accountId, item.tableName);

        // Update counter
        (*_dynamoDbCounterMap)[item.tableName].items++;
        (*_dynamoDbCounterMap)[item.tableName].size += item.size;

        return item;
    }

    Entity::DynamoDb::Item DynamoDbDatabase::UpdateItem(Entity::DynamoDb::Item &item) const {

        item.modified = system_clock::now();

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            auto session = client->start_session();

            try {

                const Entity::DynamoDb::Table table = GetTableByRegionName(item.region, item.tableName);

                mongocxx::options::find_one_and_update opts{};
                opts.return_document(mongocxx::options::return_document::k_after);

                Entity::DynamoDb::Item dbItem = GetItemByKeys(item.region, item.tableName, item.keys);
                dbItem.modified = system_clock::now();
                dbItem.attributes = item.attributes;

                document query;
                query.append(kvp("_id", bsoncxx::oid(dbItem.oid)));

                session.start_transaction();
                const auto result = _itemCollection.find_one_and_update(query.extract(), item.ToDocument(), opts);
                session.commit_transaction();
                if (result.has_value()) {
                    item = Entity::DynamoDb::Item().FromDocument(result->view());
                    log_debug << "DynamoDb item updated, oid: " << item.oid;
                    return item;
                }
                return {};
            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.UpdateItem(item);
    }

    Entity::DynamoDb::Item DynamoDbDatabase::CreateOrUpdateItem(Entity::DynamoDb::Item &item) const {

        if (ItemExists(item)) {
            return UpdateItem(item);
        }
        return CreateItem(item);
    }

    long DynamoDbDatabase::CountItems(const std::string &region, const std::string &tableName, const std::string &prefix) const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!tableName.empty()) {
                    query.append(kvp("tableName", tableName));
                }
                if (!prefix.empty()) {
                    query.append(kvp("prefix", prefix));
                }
                return _itemCollection.count_documents(query.view());

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CountItems(region);
    }

    void DynamoDbDatabase::DeleteItem(const std::string &region, const std::string &tableName, const std::map<std::string, Entity::DynamoDb::AttributeValue> &keys) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
            auto session = client->start_session();

            try {

                // Get the item
                const Entity::DynamoDb::Item item = GetItemByKeys(region, tableName, keys);
                bsoncxx::oid id(item.oid);

                session.start_transaction();
                const auto result = _itemCollection.delete_one(make_document(kvp("_id", id)));

                // Update table
                document tableSearchQuery;
                tableSearchQuery.append(kvp("tableName", tableName));

                document tableUpdateQuery;
                tableUpdateQuery.append(kvp("$inc", make_document(kvp("itemCount", -1))));
                tableUpdateQuery.append(kvp("$inc", make_document(kvp("size", -item.size))));
                _tableCollection.update_one(tableSearchQuery.view(), tableUpdateQuery.view());

                session.commit_transaction();
                log_debug << "DynamoDB item deleted, tableName: " << tableName << " count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteItem(region, tableName, keys);
        }
    }

    long DynamoDbDatabase::DeleteItems(const std::string &region, const std::string &tableName) const {

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
            auto session = client->start_session();

            try {

                // Delete item
                session.start_transaction();
                const auto result = _itemCollection.delete_many(make_document(kvp("tableName", tableName)));

                // Update table
                document tableSearchQuery;
                tableSearchQuery.append(kvp("tableName", tableName));

                document tableUpdateQuery;
                tableUpdateQuery.append(kvp("$set", make_document(kvp("itemCount", 0))));
                tableUpdateQuery.append(kvp("$set", make_document(kvp("size", 0))));
                tableUpdateQuery.append(kvp("$set", make_document(kvp("modified", bsoncxx::types::b_date(system_clock::now())))));

                _tableCollection.update_one(tableSearchQuery.view(), tableUpdateQuery.view());
                session.commit_transaction();

                log_debug << "DynamoDB item deleted, tableName: " << tableName << " count: " << result->deleted_count();
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.DeleteItems(region, tableName);
    }

    long DynamoDbDatabase::DeleteAllItems() const {

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _itemCollection = (*client)[_databaseName]["dynamodb_item"];
                const auto result = _itemCollection.delete_many({});
                log_debug << "DynamoDB items deleted, count: " << result->deleted_count();
                return result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.DeleteAllItems();
    }

}// namespace AwsMock::Database
