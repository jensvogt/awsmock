//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/dynamodb/mapper/Mapper.h>

namespace AwsMock::Dto::DynamoDb {

    Database::Entity::DynamoDb::Table Mapper::map(const DescribeTableResponse &response) {
        Database::Entity::DynamoDb::Table tableEntity;
        tableEntity.region = response.region,
        tableEntity.name = response.tableName,
        tableEntity.arn = response.tableArn,
        tableEntity.status = TableStatusTypeToString(response.tableStatus),
        tableEntity.itemCount = response.itemCount;
        tableEntity.size = response.tableSize;
        tableEntity.provisionedThroughput.FromDocument(response.provisionedThroughput.ToDocument());

        if (!response.attributeDefinitions.empty()) {
            for (const auto &attributeDefinition: response.attributeDefinitions) {
                tableEntity.attributeDefinitions.emplace_back(map(attributeDefinition));
            }
        }

        if (!response.keySchema.empty()) {
            for (const auto &keySchemas: response.keySchema) {
                tableEntity.keySchema.emplace_back(map(keySchemas));
            }
        }
        return tableEntity;
    }

    Database::Entity::DynamoDb::Item Mapper::map(const DeleteItemRequest &request) {

        Database::Entity::DynamoDb::Item item;
        item.region = request.region;
        item.tableName = request.tableName;
        for (const auto &[fst, snd]: request.keys) {
            item.attributes[fst] = map(snd);
        }
        return item;
    }

    Database::Entity::DynamoDb::Item Mapper::map(const PutItemRequest &request) {

        Database::Entity::DynamoDb::Item item;
        item.region = request.region;
        item.tableName = request.tableName;
        for (const auto &[key, attr]: request.attributes) {
            item.attributes.emplace(key, map(attr));
        }
        return item;
    }

    std::unordered_map<std::string, AttributeValue> Mapper::map(const std::unordered_map<std::string, Database::Entity::DynamoDb::DynamoValue> &attributes) {

        std::unordered_map<std::string, AttributeValue> result;
        for (const auto &[key, value]: attributes)
            result[key] = map(value);
        return result;
    }

    // Attribute Value entity -> DTO
    AttributeValue Mapper::map(const Database::Entity::DynamoDb::DynamoValue &attributeValueEntity) {
        AttributeValue attr;

        std::visit([&attr]<typename T0>(const T0 &val) {
            using T = std::decay_t<T0>;

            if constexpr (std::is_same_v<T, std::string>) {
                attr.type = "S";
                attr.stringValue = val;

            } else if constexpr (std::is_same_v<T, double>) {
                attr.type = "N";
                attr.numberValue = DoubleToString(val);

            } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                attr.type = "B";
                const std::vector bytes(attr.binaryValue.begin(), attr.binaryValue.end());
                attr.binaryValue = bytes;

            } else if constexpr (std::is_same_v<T, bool>) {
                attr.type = "BOOL";
                attr.boolValue = val;

            } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                attr.type = "NULL";

            } else if constexpr (std::is_same_v<T, std::set<std::string>>) {
                attr.type = "SS";
                attr.stringSetValue = std::vector<std::string>(val.begin(), val.end());

            } else if constexpr (std::is_same_v<T, std::set<double>>) {
                attr.type = "NS";
                for (const auto &n: val)
                    attr.numberSetValue.push_back(std::to_string(n));

            } else if constexpr (std::is_same_v<T, Database::Entity::DynamoDb::BinarySet>) {
                attr.type = "BS";
                for (const auto &b: val.values) {
                    std::vector<uint8_t> bytes(attr.binaryValue.begin(), attr.binaryValue.end());
                    attr.binarySetValue.emplace_back(bytes);
                }

            } else if constexpr (std::is_same_v<T, Database::Entity::DynamoDb::DynamoList>) {
                attr.type = "L";
                for (const auto &el: val)
                    attr.listValue.push_back(Mapper::map(*el));

            } else if constexpr (std::is_same_v<T, Database::Entity::DynamoDb::DynamoMap>) {
                attr.type = "M";
                for (const auto &[k, v]: val)
                    attr.mapValue[k] = Mapper::map(*v);

            } else {
                throw Core::DatabaseException("Unhandled DynamoValue type");
            }
        },
                   attributeValueEntity.value);
        return attr;
    }

    Database::Entity::DynamoDb::DynamoValue Mapper::map(const AttributeValue &attr) {
        Database::Entity::DynamoDb::DynamoValue dv;

        if (attr.type == "S") {
            dv.value.emplace<std::string>(attr.stringValue);

        } else if (attr.type == "N") {
            dv.value.emplace<double>(std::stod(attr.numberValue));

        } else if (attr.type == "B") {
            dv.value.emplace<std::vector<uint8_t>>(attr.binaryValue.begin(), attr.binaryValue.end());

        } else if (attr.type == "BOOL") {
            dv.value.emplace<bool>(attr.boolValue);

        } else if (attr.type == "NULL") {
            dv.value.emplace<std::nullptr_t>(nullptr);

        } else if (attr.type == "SS") {
            dv.value.emplace<std::set<std::string>>(attr.stringSetValue.begin(), attr.stringSetValue.end());

        } else if (attr.type == "NS") {
            std::set<double> ns;
            for (const auto &n: attr.numberSetValue)
                ns.insert(std::stod(n));
            dv.value.emplace<std::set<double>>(std::move(ns));

        } else if (attr.type == "BS") {
            Database::Entity::DynamoDb::BinarySet bs;
            for (const auto &b: attr.binarySetValue)
                bs.values.emplace_back(b.begin(), b.end());
            dv.value.emplace<Database::Entity::DynamoDb::BinarySet>(std::move(bs));

        } else if (attr.type == "L") {
            Database::Entity::DynamoDb::DynamoList list;
            for (const auto &el: attr.listValue)
                list.push_back(std::make_shared<Database::Entity::DynamoDb::DynamoValue>(map(el)));
            dv.value.emplace<Database::Entity::DynamoDb::DynamoList>(std::move(list));

        } else if (attr.type == "M") {
            Database::Entity::DynamoDb::DynamoMap dynMap;
            for (const auto &[k, v]: attr.mapValue)
                dynMap[k] = std::make_shared<Database::Entity::DynamoDb::DynamoValue>(map(v));
            dv.value.emplace<Database::Entity::DynamoDb::DynamoMap>(std::move(dynMap));

        } else {
            throw Core::DatabaseException("Unknown DynamoDB attribute type: " + attr.type);
        }

        return dv;
    }

    AttributeValue Mapper::map(const Database::Entity::DynamoDb::KeyValue &keyValue) {
        AttributeValue result = {};
        if (keyValue.index() == 0) {
            result.stringValue = std::get<std::string>(keyValue);
        } else if (keyValue.index() == 1) {
            result.numberValue = std::to_string(std::get<double>(keyValue));
        } else if (keyValue.index() == 2) {
            result.binaryValue = std::get<std::vector<uint8_t>>(keyValue);
        }
        return result;
    }


    // Attribute Value item entity -> DTO
    Item Mapper::map(const Database::Entity::DynamoDb::Item &itemEntity) {
        Item itemDto;
        itemDto.oid = itemEntity.oid;
        itemDto.tableName = itemEntity.tableName;
        itemDto.created = itemEntity.created;
        itemDto.modified = itemEntity.modified;
        for (const auto &[fst, snd]: itemEntity.attributes) {
            itemDto.attributes[fst] = map(snd);
        }
        return itemDto;
    }

    // Attribute Value item DTO -> entity
    Database::Entity::DynamoDb::Item Mapper::map(const Item &itemDto) {
        Database::Entity::DynamoDb::Item itemEntity;
        itemEntity.oid = itemDto.oid;
        itemEntity.tableName = itemDto.tableName;
        itemEntity.created = itemDto.created;
        itemEntity.modified = itemDto.modified;
        for (const auto &[fst, snd]: itemDto.attributes) {
            itemEntity.attributes[fst] = map(snd);
        }
        return itemEntity;
    }

    // Attribute value item list -> DTO list
    std::vector<Item> Mapper::map(const std::vector<Database::Entity::DynamoDb::Item> &itemEntities) {
        std::vector<Item> items;
        for (const auto &i: itemEntities) {
            items.emplace_back(map(i));
        }
        return items;
    }

    // Key schema DTO -> entity
    Database::Entity::DynamoDb::KeySchema Mapper::map(const KeySchema &keySchemaDto) {
        Database::Entity::DynamoDb::KeySchema keySchemaEntity;
        keySchemaEntity.attributeName = keySchemaDto.attributeName;
        keySchemaEntity.keyType = keySchemaDto.keyType;
        return keySchemaEntity;
    }

    // Key schema entity -> DTO
    KeySchema Mapper::map(const Database::Entity::DynamoDb::KeySchema &keySchemaEntity) {
        KeySchema keySchemaDto;
        keySchemaDto.attributeName = keySchemaEntity.attributeName;
        keySchemaDto.keyType = keySchemaEntity.keyType;
        return keySchemaDto;
    }

    std::vector<KeySchema> Mapper::map(const std::vector<Database::Entity::DynamoDb::KeySchema> &keySchemaEntities) {
        std::vector<KeySchema> keySchemasDtos;
        for (const auto &keySchemaEntity: keySchemaEntities) {
            keySchemasDtos.emplace_back(map(keySchemaEntity));
        }
        return keySchemasDtos;
    }

    // Attribute definition entity -> DTO
    Database::Entity::DynamoDb::AttributeDefinition Mapper::map(const AttributeDefinition &attributeDefinitionDto) {
        Database::Entity::DynamoDb::AttributeDefinition attributeDefinitionEntity;
        attributeDefinitionEntity.attributeName = attributeDefinitionDto.attributeName;
        attributeDefinitionEntity.attributeType = attributeDefinitionDto.attributeType;
        return attributeDefinitionEntity;
    }

    // Attribute definition DTO -> entity
    AttributeDefinition Mapper::map(const Database::Entity::DynamoDb::AttributeDefinition &attributeDefinitionEntity) {
        AttributeDefinition attributeDefinitionDto;
        attributeDefinitionDto.attributeName = attributeDefinitionEntity.attributeName;
        attributeDefinitionDto.attributeType = attributeDefinitionEntity.attributeType;
        return attributeDefinitionDto;
    }

    std::vector<AttributeDefinition> Mapper::map(const std::vector<Database::Entity::DynamoDb::AttributeDefinition> &attributeDefinitions) {
        std::vector<AttributeDefinition> attributeDefinitionsDto;
        for (const auto &attributeDefinition: attributeDefinitions) {
            attributeDefinitionsDto.emplace_back(map(attributeDefinition));
        }
        return attributeDefinitionsDto;
    }

    // Attribute value DTO -> entity
    std::map<std::string, Database::Entity::DynamoDb::DynamoValue> Mapper::map(const std::map<std::string, AttributeValue> &keyDtos) {
        std::map<std::string, Database::Entity::DynamoDb::DynamoValue> attributeValueEntities;
        for (const auto &[fst, snd]: keyDtos) {
            attributeValueEntities[fst] = map(snd);
        }
        return attributeValueEntities;
    }

    // Attribute Value item -> DTO
    ItemCounter Mapper::mapCounter(const Database::Entity::DynamoDb::Item &itemEntity) {
        ItemCounter itemCounterDto;
        itemCounterDto.id = itemEntity.oid;
        itemCounterDto.tableName = itemEntity.tableName;
        itemCounterDto.size = itemEntity.size;
        itemCounterDto.partitionKey = KeyValueToString(itemEntity.partitionKey);
        itemCounterDto.sortKey = KeyValueToString(itemEntity.sortKey);
        itemCounterDto.created = itemEntity.created;
        itemCounterDto.modified = itemEntity.modified;
        for (const auto &[fst, snd]: itemEntity.attributes) {
            itemCounterDto.attributes[fst] = map(snd);
        }
        return itemCounterDto;
    }

    // Attribute value item list -> DTO list
    std::vector<ItemCounter> Mapper::mapCounter(const std::vector<Database::Entity::DynamoDb::Item> &itemEntities) {
        std::vector<ItemCounter> itemCounters;
        for (const auto &i: itemEntities) {
            itemCounters.emplace_back(mapCounter(i));
        }
        return itemCounters;
    }

    std::string Mapper::DoubleToString(const double val) {
        if (val == std::floor(val) && std::abs(val) < 1e15) {
            return std::to_string(static_cast<long long>(val));
        }
        // Remove trailing zeros for non-integers
        std::string s = std::to_string(val);
        s.erase(s.find_last_not_of('0') + 1);
        if (s.back() == '.') s.pop_back();
        return s;
    }

    std::string Mapper::KeyValueToString(const KeyValue &value) {
        return std::visit([]<typename T0>(const T0 &v) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<T0>, bool>)
                return v ? "true" : "false";
            else if constexpr (std::is_same_v<std::decay_t<T0>, std::string>)
                return v;
            else if constexpr (std::is_same_v<std::decay_t<T0>, std::vector<unsigned char>>)
                return std::string(v.begin(), v.end());
            else if constexpr (std::is_arithmetic_v<std::decay_t<T0>>)
                return std::to_string(v);
            else {
                std::ostringstream oss;
                oss << v;
                return oss.str();
            }
        },
                          value);
    }
}// namespace AwsMock::Dto::DynamoDb