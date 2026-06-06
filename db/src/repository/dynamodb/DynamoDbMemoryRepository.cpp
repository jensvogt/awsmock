//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/dynamodb/DynamoDbMemoryRepository.h>

namespace Awsmock::Database {

    boost::mutex DynamoDbMemoryRepository::_tableMutex;
    boost::mutex DynamoDbMemoryRepository::_itemMutex;

    template<typename Map, typename Key>
    bool KeyCompare(Map const &lhs, Map const &rhs, Key const &keys) {

        auto pred = [](const auto &a, const auto &b) { return a.first == b.first; };

        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
    }

    bool DynamoDbMemoryRepository::tableExists(const std::string &region, const std::string &tableName) const {

        if (!region.empty()) {
            return std::ranges::find_if(_tables,
                                        [region, tableName](const std::pair<std::string, Entity::DynamoDb::Table> &table) {
                                            return table.second.region == region && table.second.name == tableName;
                                        }) != _tables.end();
        }
        return std::ranges::find_if(_tables,
                                    [tableName](const std::pair<std::string, Entity::DynamoDb::Table> &table) {
                                        return table.second.name == tableName;
                                    }) != _tables.end();
    }

    Entity::DynamoDb::TableList DynamoDbMemoryRepository::listTables(const std::string &region, const std::string &prefix, int pageSize, int pageIndex, const std::vector<SortColumn> &sortColumns) const {

        Entity::DynamoDb::TableList tables;
        if (region.empty()) {
            for (const auto &val: _tables | std::views::values) {
                tables.emplace_back(val);
            }
        } else {
            for (const auto &val: _tables | std::views::values) {
                if (val.region == region) {
                    tables.emplace_back(val);
                }
            }
        }

        log_trace << "Got DynamoDB table, size: " << tables.size();
        return tables;
    }

    Entity::DynamoDb::Table DynamoDbMemoryRepository::createTable(Entity::DynamoDb::Table &table) const {
        boost::mutex::scoped_lock lock(_tableMutex);

        table.oid = Core::StringUtils::CreateRandomUuid();
        _tables[table.oid] = table;
        log_trace << "DynamoDB table created, oid: " << table.oid;
        return _tables[table.oid];
    }

    Entity::DynamoDb::Table DynamoDbMemoryRepository::getTableById(const std::string &oid) const {

        const auto it =
                std::ranges::find_if(_tables, [oid](const std::pair<std::string, Entity::DynamoDb::Table> &table) {
                    return table.first == oid;
                });

        if (it == _tables.end()) {
            log_error << "Get table by ID failed, oid: " << oid;
            throw Core::DatabaseException("Get table by ID failed, oid: " + oid);
        }

        it->second.oid = oid;
        return it->second;
    }

    Entity::DynamoDb::Table DynamoDbMemoryRepository::getTableById(const bsoncxx::oid &oid) const {
        return getTableById(oid.to_string());
    }

    Entity::DynamoDb::Table DynamoDbMemoryRepository::getTableByRegionName(const std::string &region, const std::string &name) const {

        const auto it = std::ranges::find_if(_tables,
                                             [region, name](const std::pair<std::string, Entity::DynamoDb::Table> &table) {
                                                 return table.second.region == region && table.second.name == name;
                                             });

        if (it == _tables.end()) {
            log_error << "Get table by region and name failed, region: " << region << " name: " << name;
            throw Core::DatabaseException("Get table by region and name failed, region: " + region + " name: " + name);
        }

        it->second.oid = it->first;
        return it->second;
    }

    Entity::DynamoDb::Table DynamoDbMemoryRepository::updateTable(Entity::DynamoDb::Table &table) const {
        boost::mutex::scoped_lock lock(_tableMutex);

        std::string region = table.region;
        std::string name = table.name;
        const auto it = std::ranges::find_if(_tables,
                                             [region, name](const std::pair<std::string, Entity::DynamoDb::Table> &table) {
                                                 return table.second.region == region && table.second.name == name;
                                             });
        _tables[it->first] = table;
        return _tables[it->first];
    }

    Entity::DynamoDb::Table DynamoDbMemoryRepository::createOrUpdateTable(Entity::DynamoDb::Table &table) const {
        if (tableExists(table.region, table.name)) {
            return updateTable(table);
        }
        return createTable(table);
    }

    void DynamoDbMemoryRepository::updateTableCounter(const std::string &tableArn, const long items, const long size) const {

        boost::mutex::scoped_lock lock(_tableMutex);

        const auto it = std::ranges::find_if(_tables,
                                             [tableArn](const std::pair<std::string, Entity::DynamoDb::Table> &t) {
                                                 return t.second.arn == tableArn;
                                             });
        if (it != _tables.end()) {
            it->second.items = items;
            it->second.size = size;
        }
    }

    void DynamoDbMemoryRepository::deleteTable(const std::string &region, const std::string &tableName) const {
        boost::mutex::scoped_lock lock(_tableMutex);

        const auto count = std::erase_if(_tables, [region, tableName](const auto &item) {
            auto const &[key, value] = item;
            return value.region == region && value.name == tableName;
        });
        log_debug << "DynamoDB table deleted, count: " << count;
    }

    long DynamoDbMemoryRepository::deleteAllTables() const {
        boost::mutex::scoped_lock lock(_tableMutex);

        const long count = static_cast<long>(_tables.size());
        log_debug << "All DynamoDb tables deleted, count: " << count;
        _tables.clear();
        return count;
    }

    bool DynamoDbMemoryRepository::itemExists(const Entity::DynamoDb::Item &item) const {
        return std::ranges::find_if(_items,
                                    [item](const std::pair<std::string, Entity::DynamoDb::Item> &i) {
                                        return i.second.region == item.region && i.second.tableName == item.tableName && i.second.partitionKey == item.partitionKey && i.second.sortKey == item.sortKey;
                                    }) != _items.end();
    }

    bool DynamoDbMemoryRepository::itemExists(const std::string &region, const std::string &tableName, std::string &partitionKey, const std::string &sortKey) const {
        return std::ranges::find_if(_items,
                                    [region, tableName, partitionKey, sortKey](const std::pair<std::string, Entity::DynamoDb::Item> &i) {
                                        return i.second.region == region && i.second.tableName == tableName && std::get<0>(i.second.partitionKey) == partitionKey && std::get<0>(i.second.sortKey) == sortKey;
                                    }) != _items.end();
    }

    Entity::DynamoDb::ItemList DynamoDbMemoryRepository::listItems(const std::string &region, const std::string &tableName, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        auto q = Core::from(Core::NumberUtils::toVector(_items));
        if (!region.empty()) {
            q = q.where([region](const Entity::DynamoDb::Item &item) { return item.region == region; });
        }
        if (!tableName.empty()) {
            q = q.where([tableName](const Entity::DynamoDb::Item &item) { return item.tableName == tableName; });
        }
        log_trace << "Got DynamoDB items, region: " << region << ", tableName: " << tableName << ", size: " << q.count();
        return q.to_vector();
    }

    long DynamoDbMemoryRepository::countTables(const std::string &region, const std::string &prefix) const {

        if (!region.empty()) {

            long count = 0;
            for (const auto &val: _tables | std::views::values) {
                if (val.region == region) {
                    count++;
                }
            }
            return count;
        }
        return static_cast<long>(_tables.size());
    }

    Entity::DynamoDb::Item DynamoDbMemoryRepository::getItemById(const std::string &oid) const {

        const auto it =
                std::ranges::find_if(_items, [oid](const std::pair<std::string, Entity::DynamoDb::Item> &item) {
                    return item.first == oid;
                });

        if (it == _items.end()) {
            log_error << "Get item by ID failed, oid: " << oid;
            throw Core::DatabaseException("Get item by ID failed, oid: " + oid);
        }

        it->second.oid = oid;
        return it->second;
    }

    Entity::DynamoDb::Item DynamoDbMemoryRepository::getItemById(const bsoncxx::oid &oid) const {
        return getItemById(oid.to_string());
    }

    Entity::DynamoDb::Item DynamoDbMemoryRepository::getItemByKeys(const std::string &region, const std::string &tableName, const Entity::DynamoDb::KeyValue &partitionKey, const Entity::DynamoDb::KeyValue &sortKey) const {

        const auto it =
                std::ranges::find_if(_items, [region, tableName, partitionKey, sortKey](const std::pair<std::string, Entity::DynamoDb::Item> &item) {
                    return item.second.region == region && item.second.tableName == tableName && item.second.partitionKey == partitionKey && item.second.sortKey == sortKey;
                });

        if (it == _items.end()) {
            log_error << "Get item by keys failed, region: " << region << ", tableName: " << tableName;
            throw Core::DatabaseException("Get item by keys failed, region: " + region + ", tableName: " + tableName);
        }
        return it->second;
    }

    Entity::DynamoDb::Item DynamoDbMemoryRepository::createItem(Entity::DynamoDb::Item &item) const {
        boost::mutex::scoped_lock lock(_itemMutex);

        item.oid = Core::StringUtils::CreateRandomUuid();
        _items[item.oid] = item;
        log_trace << "DynamoDB item created, oid: " << item.oid;
        return _items[item.oid];
    }

    Entity::DynamoDb::Item DynamoDbMemoryRepository::updateItem(Entity::DynamoDb::Item &item) const {
        boost::mutex::scoped_lock lock(_itemMutex);
        const auto it = std::ranges::find_if(_items,
                                             [item](const std::pair<std::string, Entity::DynamoDb::Item> &i) {
                                                 return i.second.region == item.region && i.second.tableName == item.tableName && i.second.partitionKey == item.partitionKey && i.second.sortKey == item.sortKey;
                                             });
        if (it != _items.end()) {
            _items[it->first] = item;
            return _items[it->first];
        }
        return {};
    }

    Entity::DynamoDb::Item DynamoDbMemoryRepository::createOrUpdateItem(Entity::DynamoDb::Item &item) const {
        if (itemExists(item)) {
            return updateItem(item);
        }
        return createItem(item);
    }

    long DynamoDbMemoryRepository::countItems(const std::string &region, const std::string &tableName, const std::string &prefix) const {

        if (!region.empty()) {
            long count = 0;
            for (const auto &val: _items | std::views::values) {
                if (val.region == region) {
                    count++;
                }
            }
            return count;
        }
        return static_cast<long>(_items.size());
    }

    std::vector<Entity::DynamoDb::Item> DynamoDbMemoryRepository::getItems(const std::string &region, const std::string &tableName) const {

        auto q = Core::from(Core::NumberUtils::toVector(_items));
        if (!region.empty()) {
            q = q.where([region](const Entity::DynamoDb::Item &item) { return item.region == region; });
        }
        if (!tableName.empty()) {
            q = q.where([tableName](const Entity::DynamoDb::Item &item) { return item.tableName == tableName; });
        }
        return q.to_vector();
    }

    std::vector<Entity::DynamoDb::Item> DynamoDbMemoryRepository::executeQuery(const value &filter, bool scanIndexForward, int limit) const {
        return {};
    }

    std::vector<Entity::DynamoDb::Item> DynamoDbMemoryRepository::executeQuery(const DynamoToMongoTranslator::DynamoRequest &req, bool scanIndexForward, int limit) const {
        return {};
    }

    void DynamoDbMemoryRepository::adjustItemCounters() const {

        for (auto &table: _tables | std::views::values) {

            // Count items belonging to this table
            table.items = static_cast<long>(std::ranges::count_if(_items, [&table](const auto &pair) {
                return pair.second.tableName == table.name;
            }));

            // Sum size of items belonging to this table
            table.size = std::accumulate(_items.begin(), _items.end(), 0L,
                                         [&table](long sum, const auto &pair) {
                                             return pair.second.tableName == table.name ? sum + pair.second.size : sum;
                                         });
        }
    }

    void DynamoDbMemoryRepository::deleteItem(const std::string &region, const std::string &tableName, const std::string &partitionKey, const std::string &sortKey) const {
        boost::mutex::scoped_lock lock(_itemMutex);

        const auto count = std::erase_if(_items, [region, tableName, partitionKey, sortKey](const auto &item) {
            return item.second.region == region && item.second.tableName == tableName && item.second.partitionKey == Entity::DynamoDb::KeyValue{partitionKey} && item.second.sortKey == Entity::DynamoDb::KeyValue{sortKey};
        });
        log_debug << "DynamoDB items deleted, count: " << count;
    }

    long DynamoDbMemoryRepository::deleteItems(const std::string &region, const std::string &tableName) const {
        boost::mutex::scoped_lock lock(_itemMutex);

        const auto count = std::erase_if(_items, [region, tableName](const auto &item) {
            return item.second.region == region && item.second.tableName == tableName;
        });
        log_debug << "DynamoDB items deleted, tableName: " << tableName << " count: " << count;
        return static_cast<long>(count);
    }

    long DynamoDbMemoryRepository::deleteAllItems() const {
        boost::mutex::scoped_lock lock(_itemMutex);

        const long count = static_cast<long>(_items.size());
        log_debug << "DynamoDB items deleted, count: " << _items.size();
        _items.clear();
        return count;
    }
} // namespace Awsmock::Database
