//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/DynamoDbMemoryDb.h>
#include <boost/accumulators/statistics/count.hpp>

namespace AwsMock::Database {

    boost::mutex DynamoDbMemoryDb::_tableMutex;
    boost::mutex DynamoDbMemoryDb::_itemMutex;

    template<typename Map, typename Key>
    bool KeyCompare(Map const &lhs, Map const &rhs, Key const &keys) {

        auto pred = [](auto a, auto b) { return a.first == b.first; };

        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
    }

    bool DynamoDbMemoryDb::TableExists(const std::string &region, const std::string &tableName) {

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

    Entity::DynamoDb::TableList DynamoDbMemoryDb::ListTables(const std::string &region) {

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

    Entity::DynamoDb::Table DynamoDbMemoryDb::CreateTable(const Entity::DynamoDb::Table &table) {
        boost::mutex::scoped_lock lock(_tableMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _tables[oid] = table;
        log_trace << "Lambda created, oid: " << oid;
        return GetTableById(oid);
    }

    Entity::DynamoDb::Table DynamoDbMemoryDb::GetTableById(const std::string &oid) {

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

    Entity::DynamoDb::Table DynamoDbMemoryDb::GetTableByRegionName(const std::string &region, const std::string &name) {

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

    Entity::DynamoDb::Table DynamoDbMemoryDb::UpdateTable(const Entity::DynamoDb::Table &table) {
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

    void DynamoDbMemoryDb::UpdateTableCounter(const std::string &tableArn, const long items, const long size) {

        boost::mutex::scoped_lock lock(_tableMutex);

        const auto it = std::ranges::find_if(_tables,
                                             [tableArn](const std::pair<std::string, Entity::DynamoDb::Table> &t) {
                                                 return t.second.arn == tableArn;
                                             });
        if (it != _tables.end()) {
            it->second.itemCount = items;
            it->second.size = size;
        }
    }

    void DynamoDbMemoryDb::DeleteTable(const std::string &tableName) {
        boost::mutex::scoped_lock lock(_tableMutex);

        const auto count = std::erase_if(_tables, [tableName](const auto &item) {
            auto const &[key, value] = item;
            return value.name == tableName;
        });
        log_debug << "DynamoDB table deleted, count: " << count;
    }

    long DynamoDbMemoryDb::DeleteAllTables() {
        boost::mutex::scoped_lock lock(_tableMutex);

        const long count = _tables.size();
        log_debug << "All DynamoDb tables deleted, count: " << count;
        _tables.clear();
        return count;
    }

    bool DynamoDbMemoryDb::ItemExists(const Entity::DynamoDb::Item &item) {

        const std::string region = item.region;
        std::string tableName = item.tableName;

        // Get table
        Entity::DynamoDb::Table table = GetTableByRegionName(region, tableName);

        if (!region.empty()) {
            return std::ranges::find_if(_items,
                                        [table, item](const std::pair<std::string, Entity::DynamoDb::Item> &item1) {
                                            bool result = item1.second.region == table.region && item1.second.tableName == table.name;
                                            result &= KeyCompare(item.attributes, item1.second.attributes, table.keySchemas);
                                            return result;
                                        }) != _items.end();
        }
        return std::ranges::find_if(_items,
                                    [tableName](const std::pair<std::string, Entity::DynamoDb::Item> &item) {
                                        return item.second.tableName == tableName;
                                    }) != _items.end();
    }

    Entity::DynamoDb::ItemList DynamoDbMemoryDb::ListItems(const std::string &region, const std::string &tableName) const {

        Entity::DynamoDb::ItemList items;
        if (region.empty() && tableName.empty()) {

            for (const auto &val: _items | std::views::values) {
                items.emplace_back(val);
            }

        } else if (tableName.empty()) {

            for (const auto &val: _items | std::views::values) {
                if (val.region == region) {
                    items.emplace_back(val);
                }
            }

        } else {

            for (const auto &val: _items | std::views::values) {
                if (val.region == region && val.tableName == tableName) {
                    items.emplace_back(val);
                }
            }
        }

        log_trace << "Got DynamoDB items, size: " << items.size();
        return items;
    }

    long DynamoDbMemoryDb::GetTableSize(const std::string &region, const std::string &tableName) const {

        long size = 0;
        if (region.empty() && tableName.empty()) {

            for (const auto &val: _items | std::views::values) {
                size += val.size;
            }

        } else if (tableName.empty()) {

            for (const auto &val: _items | std::views::values) {
                if (val.region == region) {
                    size += val.size;
                }
            }

        } else {

            for (const auto &val: _items | std::views::values) {
                if (val.region == region && val.tableName == tableName) {
                    size += val.size;
                }
            }
        }

        log_trace << "Got DynamoDB size, region: " << region << ", tableName: " << tableName << ", size: " << size;
        return size;
    }

    long DynamoDbMemoryDb::CountTables(const std::string &region) const {

        if (!region.empty()) {

            long count = 0;
            for (const auto &val: _tables | std::views::values) {
                if (val.region == region) {
                    count++;
                }
            }
            return count;

        } else {

            return static_cast<long>(_tables.size());
        }
    }

    Entity::DynamoDb::Item DynamoDbMemoryDb::GetItemById(const std::string &oid) {

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

    Entity::DynamoDb::Item DynamoDbMemoryDb::CreateItem(const Entity::DynamoDb::Item &item) {
        boost::mutex::scoped_lock lock(_itemMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _items[oid] = item;
        log_trace << "Item created, oid: " << oid;
        return GetItemById(oid);
    }

    Entity::DynamoDb::Item DynamoDbMemoryDb::UpdateItem(const Entity::DynamoDb::Item &item) {
        boost::mutex::scoped_lock lock(_itemMutex);

        std::string region = item.region;
        std::string tableName = item.tableName;
        const auto it = std::ranges::find_if(_items,
                                             [region, tableName](const std::pair<std::string, Entity::DynamoDb::Item> &item) {
                                                 return item.second.region == region && item.second.tableName == tableName;
                                             });
        _items[it->first] = item;
        return _items[it->first];
    }

    long DynamoDbMemoryDb::CountItems(const std::string &region) const {

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

    void DynamoDbMemoryDb::DeleteItem(const std::string &region, const std::string &tableName, const std::map<std::string, Entity::DynamoDb::AttributeValue> &keys) {
        boost::mutex::scoped_lock lock(_itemMutex);

        const auto count = std::erase_if(_items, [region, tableName, keys](const auto &item) {
            auto const &[k, v] = item;
            return v.region == region && v.tableName == tableName;
        });
        log_debug << "DynamoDB items deleted, count: " << count;
    }

    void DynamoDbMemoryDb::DeleteItems(const std::string &region, const std::string &tableName) {
        boost::mutex::scoped_lock lock(_itemMutex);

        const auto count = std::erase_if(_items, [region, tableName](const auto &item) {
            auto const &[k, v] = item;
            return v.region == region && v.tableName == tableName;
        });
        log_debug << "DynamoDB items deleted, tableName: " << tableName << " count: " << count;
    }

    long DynamoDbMemoryDb::DeleteAllItems() {
        boost::mutex::scoped_lock lock(_itemMutex);

        long count = _items.size();
        log_debug << "DynamoDB items deleted, count: " << _items.size();
        _items.clear();
        return count;
    }

}// namespace AwsMock::Database