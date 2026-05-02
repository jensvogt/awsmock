//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/DynamoDbDatabase.h>

namespace AwsMock::Database {

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
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "table_exists");

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

    std::vector<Entity::DynamoDb::Table> DynamoDbDatabase::ListTables(const std::string &region, const std::string &prefix, const int pageSize, const int pageIndex, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "list_tables");

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

                if (!sortColumns.empty()) {
                    document sort;
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
                    }
                    opts.sort(sort.extract());
                }

                for (auto tableCursor = _tableCollection.find(query.view(), opts); auto table: tableCursor) {
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
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "count_tables");

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
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "get_table_size");

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
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "update_table");

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
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "update_table");

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
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "delete_table");

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
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "delete_all_tables");

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
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "item_exists");

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];

                // Set limit to 1 (Very important for performance!)
                mongocxx::options::count options;
                options.limit(1);

                document query;
                if (!item.region.empty()) {
                    query.append(kvp("region", item.region));
                }
                if (!item.tableName.empty()) {
                    query.append(kvp("tableName", item.tableName));
                }

                // Partition key
                if (item.partitionKey.index() == 0)
                    query.append(kvp("partitionKey", std::get<std::string>(item.partitionKey)));
                if (item.partitionKey.index() == 1)
                    query.append(kvp("partitionKey", std::get<double>(item.partitionKey)));
                // if (item.partitionKey.index() == 2)
                //     query.append(kvp("partitionKey", std::get<std::vector<uint8_t>>(item.partitionKey)));

                // Sort key
                if (item.sortKey.index() == 0)
                    query.append(kvp("sortKey", std::get<std::string>(item.sortKey)));
                if (item.sortKey.index() == 1)
                    query.append(kvp("sortKey", std::get<double>(item.sortKey)));
                // if (item.sortKey.index() == 2)
                //     query.append(kvp("sortKey", std::get<std::vector<uint8_t>>(item.sortKey)));

                return _itemCollection.count_documents(query.view(), options) > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ItemExists(item);
    }

    bool DynamoDbDatabase::ItemExists(const std::string &region, const std::string &tableName, std::string &partitionKey, const std::string &sortKey) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "item_exists");

        if (HasDatabase()) {

            try {

                const auto client = ConnectionPool::instance().GetConnection();
                mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];

                // Set limit to 1 (Very important for performance!)
                mongocxx::options::count options;
                options.limit(1);

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }
                if (!tableName.empty()) {
                    query.append(kvp("tableName", tableName));
                }

                // Primary keys
                if (!partitionKey.empty()) {
                    query.append(kvp("partitionKey", partitionKey));
                }
                if (!partitionKey.empty()) {
                    query.append(kvp("sortKey", sortKey));
                }

                return _itemCollection.count_documents(query.view(), options) > 0;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.ItemExists(region, tableName, partitionKey, sortKey);
    }

    Entity::DynamoDb::ItemList DynamoDbDatabase::ListItems(const std::string &region, const std::string &tableName, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "list_items");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            try {
                Entity::DynamoDb::ItemList items;

                document query;
                if (!region.empty()) {
                    query.append(kvp("region", region));
                }

                if (!tableName.empty()) {
                    query.append(kvp("tableName", tableName));
                }

                mongocxx::options::find opts;
                if (pageSize > 0) {
                    opts.limit(pageSize);
                    if (pageIndex > 0) {
                        opts.skip(pageSize * pageIndex);
                    }
                }

                if (!sortColumns.empty()) {
                    document sort;
                    for (const auto &[column, sortDirection]: sortColumns) {
                        sort.append(kvp(column, sortDirection));
                    }
                    opts.sort(sort.extract());
                }

                for (auto itemCursor = _itemCollection.find(query.extract(), opts); const auto item: itemCursor) {
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
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "get_item_by_id");

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

    Entity::DynamoDb::Item DynamoDbDatabase::GetItemByKeys(const std::string &region, const std::string &tableName, const Entity::DynamoDb::KeyValue &partitionKey, const Entity::DynamoDb::KeyValue &sortKey) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "get_item_by_key");

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

                // Primary keys
                if (partitionKey.index() == 0) {
                    query.append(kvp("partitionKey", std::get<std::string>(partitionKey)));
                }

                if (sortKey.index() == 0) {
                    query.append(kvp("sortKey", std::get<std::string>(sortKey)));
                } else if (sortKey.index() == 1) {
                    query.append(kvp("sortKey", std::get<double>(sortKey)));
                    // } else if (sortKey.index() == 2) {
                    //     query.append(kvp("sortKey", std::get<std::vector<uint8_t>>(sortKey)));
                }

                if (const auto mResult = _itemCollection.find_one(query.view())) {
                    Entity::DynamoDb::Item result;
                    result.FromDocument(mResult->view());
                    log_debug << "Got item by ID, item: " << result.ToString();
                    return result;
                }
                log_error << "Database exception: item not found, query: " << bsoncxx::to_json(query);
                throw Core::DatabaseException("Database exception, item not found, region: " + region + ", tableName: " + tableName);

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.GetItemByKeys(region, tableName, partitionKey, sortKey);
    }

    Entity::DynamoDb::Item DynamoDbDatabase::CreateItem(Entity::DynamoDb::Item &item) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "create_item");

        item.created = system_clock::now();
        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            auto session = client->start_session();

            try {

                session.start_transaction();
                const auto doc = item.ToDocument();// owned document
                const auto view = doc.view();      // non-owning view into doc
                item.size = static_cast<long>(view.length()) + sizeof(long);
                const auto itemResult = _itemCollection.insert_one(view);
                session.commit_transaction();

                log_trace << "DynamoDb item created, oid: " << itemResult->inserted_id().get_oid().value.to_string();
                item.oid = itemResult->inserted_id().get_oid().value.to_string();
                return item;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.CreateItem(item);
    }

    Entity::DynamoDb::Item DynamoDbDatabase::UpdateItem(Entity::DynamoDb::Item &item) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "update_item");

        item.modified = system_clock::now();

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            auto session = client->start_session();

            try {
                mongocxx::options::find_one_and_update opts{};
                opts.return_document(mongocxx::options::return_document::k_after);

                session.start_transaction();
                const Entity::DynamoDb::Table table = GetTableByRegionName(item.region, item.tableName);

                // Get partitionKey/sortKey from attributes
                Entity::DynamoDb::KeyValue partitionKey = DynamoVariantToKeyValue(item.attributes[table.GetPartitionKeyName()].value);
                Entity::DynamoDb::KeyValue sortKey;
                if (!table.GetSortKeyName().empty()) {
                    sortKey = DynamoVariantToKeyValue(item.attributes[table.GetSortKeyName()].value);
                }

                // Get the database item
                Entity::DynamoDb::Item dbItem = GetItemByKeys(item.region, item.tableName, partitionKey, sortKey);
                dbItem.modified = system_clock::now();
                dbItem.attributes = item.attributes;

                // document query;
                document query;
                query.append(kvp("_id", bsoncxx::oid(dbItem.oid)));

                const auto view = dbItem.ToDocument().view();
                dbItem.size = static_cast<long>(view.length() + sizeof(long));

                const auto result = _itemCollection.find_one_and_update(query.extract(), dbItem.ToDocument(), opts);
                session.commit_transaction();
                if (result.has_value()) {
                    item.FromDocument(result->view());
                    log_debug << "DynamoDb item updated, oid: " << item.oid;
                    return item;
                }
                return {};
            } catch (const Core::DatabaseException &exc) {
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
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "count_items");

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

    void DynamoDbDatabase::DeleteItem(const std::string &region, const std::string &tableName, const std::string &partitionKey, const std::string &sortKey) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "delete_item");

        if (HasDatabase()) {

            const auto client = ConnectionPool::instance().GetConnection();
            mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];
            mongocxx::collection _tableCollection = (*client)[_databaseName][_tableCollectionName];
            auto session = client->start_session();

            try {

                // Get the item
                const Entity::DynamoDb::Item item = GetItemByKeys(region, tableName, partitionKey, sortKey);
                bsoncxx::oid id(item.oid);

                session.start_transaction();
                const auto result = _itemCollection.delete_one(make_document(kvp("_id", id)));

                // Update table
                document tableSearchQuery;
                tableSearchQuery.append(kvp("tableName", tableName));

                document tableUpdateQuery;
                tableUpdateQuery.append(kvp("$inc", make_document(kvp("itemCount", bsoncxx::types::b_int64(-1)))));
                tableUpdateQuery.append(kvp("$inc", make_document(kvp("size", bsoncxx::types::b_int64(-item.size)))));
                _tableCollection.update_one(tableSearchQuery.view(), tableUpdateQuery.view());

                session.commit_transaction();
                log_debug << "DynamoDB item deleted, tableName: " << tableName << " count: " << result->deleted_count();

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }

        } else {

            _memoryDb.DeleteItem(region, tableName, partitionKey, sortKey);
        }
    }

    long DynamoDbDatabase::DeleteItems(const std::string &region, const std::string &tableName) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "delete_items");

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
                tableUpdateQuery.append(kvp("$set", make_document(kvp("itemCount", bsoncxx::types::b_int64()))));
                tableUpdateQuery.append(kvp("$set", make_document(kvp("size", bsoncxx::types::b_int64()))));
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
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "delete_all_items");

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

    std::vector<Entity::DynamoDb::Item> DynamoDbDatabase::GetItems(const std::string &region, const std::string &tableName) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "get_items");

        std::vector<Entity::DynamoDb::Item> items{};
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

                for (auto cursor = _itemCollection.find(query.view()); const view &doc: cursor) {
                    Entity::DynamoDb::Item item;
                    item.FromDocument(doc);
                    items.emplace_back(std::move(item));
                }

                log_debug << "DynamoDB items exported, count: " << items.size();
                return items;

            } catch (const mongocxx::exception &exc) {
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException("Database exception " + std::string(exc.what()));
            }
        }
        return _memoryDb.GetItems(region, tableName);
    }

    std::vector<Entity::DynamoDb::Item> DynamoDbDatabase::ExecuteQuery(const DynamoToMongoTranslator::DynamoRequest &req, const bool scanIndexForward, const int limit) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "execute_query");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];

        const auto filter = DynamoToMongoTranslator::translate(req);

        mongocxx::options::find opts{};

        // DynamoDB Sort Key sorting (Ascending vs Descending). You'd need to know which field is the Sort Key from your Table Metadata
        std::string sortKey = "SK";
        opts.sort(make_document(kvp(sortKey, scanIndexForward ? 1 : -1)));

        if (limit > 0) {
            opts.limit(limit);
        }

        // Execute query
        std::vector<Entity::DynamoDb::Item> items;
        for (auto cursor = _itemCollection.find(filter.view(), opts); auto &&doc: cursor) {
            Entity::DynamoDb::Item item;
            item.FromDocument(doc);
            items.push_back(item);
        }
        return items;
    }

    std::vector<Entity::DynamoDb::Item> DynamoDbDatabase::ExecuteQuery(const value &filter, const bool scanIndexForward, const int limit) const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "execute_query");

        const auto client = ConnectionPool::instance().GetConnection();
        mongocxx::collection _itemCollection = (*client)[_databaseName][_itemCollectionName];

        mongocxx::options::find opts{};

        // DynamoDB Sort Key sorting (Ascending vs Descending). You'd need to know which field is the Sort Key from your Table Metadata
        //std::string sortKey = "SK";
        //opts.sort(make_document(kvp(sortKey, scanIndexForward ? 1 : -1)));

        if (limit > 0) {
            opts.limit(limit);
        }

        // Execute query
        long totalSize = 0;
        std::vector<Entity::DynamoDb::Item> items;
        for (auto cursor = _itemCollection.find(filter.view(), opts); auto &&doc: cursor) {
            Entity::DynamoDb::Item item;
            item.FromDocument(doc);
            items.push_back(item);
            totalSize += item.size;
            if (totalSize > QUERY_SIZE_LIMIT) {
                break;
            }
        }
        return items;
    }

    void DynamoDbDatabase::AdjustItemCounters() const {
        Monitoring::MonitoringTimer measure(DYNAMODB_DATABASE_TIMER, DYNAMODB_DATABASE_COUNTER, "action", "adjust_item_counter");

        if (HasDatabase()) {
            const auto client = ConnectionPool::instance().GetConnection();
            auto itemCollection = (*client)[_databaseName][_itemCollectionName];
            auto tableCollection = (*client)[_databaseName][_tableCollectionName];

            // Build aggregation pipeline
            mongocxx::pipeline p{};

            // Calculate bsonSize of each document
            p.add_fields(make_document(
                    kvp("bsonSize", make_document(
                                            kvp("$bsonSize", "$$ROOT")))));

            // Group by tableName, sum sizes and count items
            p.group(make_document(
                    kvp("_id", "$tableName"),
                    kvp("size", make_document(kvp("$sum", "$bsonSize"))),
                    kvp("items", make_document(kvp("$sum", 1)))));

            // Reshape output
            p.project(make_document(
                    kvp("_id", 0),
                    kvp("tableName", "$_id"),
                    kvp("size", 1),
                    kvp("items", 1)));

            auto session = client->start_session();
            session.start_transaction();

            try {
                // Reset all tables to zero
                tableCollection.update_many(
                        {},
                        make_document(kvp("$set", make_document(
                                                          kvp("size", bsoncxx::types::b_int64{0}),
                                                          kvp("items", bsoncxx::types::b_int64{0})))));

                auto bulk = tableCollection.create_bulk_write();

                for (auto cursor = itemCollection.aggregate(p); const auto &t: cursor) {

                    const auto tableName = Core::Bson::BsonUtils::GetStringValue(t, "tableName");
                    const auto size = Core::Bson::BsonUtils::GetLongValue(t, "size");
                    const auto items = Core::Bson::BsonUtils::GetLongValue(t, "items");

                    bulk.append(mongocxx::model::update_one(
                            make_document(kvp("name", tableName)),
                            make_document(kvp("$set", make_document(
                                                              kvp("size", bsoncxx::types::b_int64{size}),
                                                              kvp("items", bsoncxx::types::b_int64{items}))))));

                    log_info << "Table stats updated: " << tableName << ", size: " << size << " bytes" << ", items: " << items;
                }

                // Execute bulk write only if there are updates
                if (!bulk.empty()) {
                    auto result = bulk.execute();
                    log_debug << "Bulk write result, modified: " << result->modified_count();
                }

                session.commit_transaction();
                return;

            } catch (const mongocxx::exception &exc) {
                session.abort_transaction();
                log_error << "Database exception " << exc.what();
                throw Core::DatabaseException(exc.what());
            }
        }
        _memoryDb.AdjustItemCounters();
    }

    Entity::DynamoDb::KeyValue DynamoDbDatabase::DynamoVariantToKeyValue(const Entity::DynamoDb::DynamoValue::DynamoVariant &variant) const {
        return std::visit([]<typename T0>(const T0 &val) -> Entity::DynamoDb::KeyValue {
            using T = std::decay_t<T0>;

            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else if constexpr (std::is_same_v<T, double>) {
                return val;
            } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                return val;
            } else {
                throw Core::DatabaseException("DynamoValue type is not a valid KeyValue (S, N, or B)");
            }
        },
                          variant);
    }

    void DynamoDbDatabase::DumpVariant(const Entity::DynamoDb::Table &table, Entity::DynamoDb::Item &item) const {
        std::string tmp0 = table.GetPartitionKeyName();
        std::string tmp1 = table.GetSortKeyName();
        auto debugVariant = [](const Entity::DynamoDb::DynamoValue::DynamoVariant &variant, const std::string &name) {
            std::visit([&name, variant]<typename T0>(const T0 &val) {
                using T = std::decay_t<T0>;
                if constexpr (std::is_same_v<T, std::string>)
                    std::cerr << name << " type: string, value: " << val << "\n";
                else if constexpr (std::is_same_v<T, double>)
                    std::cerr << name << " type: double, value: " << val << "\n";
                else if constexpr (std::is_same_v<T, std::vector<uint8_t>>)
                    std::cerr << name << " type: binary\n";
                else if constexpr (std::is_same_v<T, bool>)
                    std::cerr << name << " type: bool, value: " << val << "\n";
                else if constexpr (std::is_same_v<T, std::nullptr_t>)
                    std::cerr << name << " type: null\n";
                else if constexpr (std::is_same_v<T, std::set<std::string>>)
                    std::cerr << name << " type: SS\n";
                else if constexpr (std::is_same_v<T, std::set<double>>)
                    std::cerr << name << " type: NS\n";
                else
                    std::cerr << name << " type: unknown index: " << variant.index() << "\n";
            },
                       variant);
        };

        debugVariant(item.attributes[table.GetPartitionKeyName()].value, "partitionKey");
        debugVariant(item.attributes[table.GetSortKeyName()].value, "sortKey");
    }
}// namespace AwsMock::Database
