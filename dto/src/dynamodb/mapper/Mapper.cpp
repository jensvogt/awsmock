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
        tableEntity.attributes = response.attributes,
        tableEntity.keySchemas = response.keySchemas;
        tableEntity.itemCount = response.itemCount;
        tableEntity.size = response.tableSize;
        tableEntity.provisionedThroughput.FromDocument(response.provisionedThroughput.ToDocument());
        return tableEntity;
    }

    Database::Entity::DynamoDb::Item Mapper::map(const DeleteItemRequest &request) {

        Database::Entity::DynamoDb::Item item;
        item.region = request.region;
        item.tableName = request.tableName;

        for (const auto &[fst, snd]: request.keys) {
            Database::Entity::DynamoDb::AttributeValue attributeValue;
            attributeValue.stringValue = snd.stringValue;
            attributeValue.numberValue = snd.numberValue;
            item.keys[fst] = attributeValue;
        }

        return item;
    }

    Database::Entity::DynamoDb::Item Mapper::map(const PutItemRequest &request) {

        Database::Entity::DynamoDb::Item item;
        item.region = request.region;
        item.tableName = request.tableName;

        for (const auto &[fst, snd]: request.attributes) {
            Database::Entity::DynamoDb::AttributeValue attribute;
            attribute.stringValue = snd.stringValue;
            attribute.stringSetValue = snd.stringSetValue;
            attribute.numberValue = snd.numberValue;
            attribute.numberSetValue = snd.numberSetValue;
            attribute.boolValue = snd.boolValue;
            attribute.nullValue = snd.nullValue;
            item.attributes[fst] = attribute;

            /*            if (table.keySchemas.contains(fst)) {
                item.keys[fst] = attribute;
            }*/
        }
        return item;
    }

    // Attribute Value entity list -> DTO list
    std::map<std::string, AttributeValue> Mapper::map(const std::map<std::string, Database::Entity::DynamoDb::AttributeValue> &attributeValue) {
        std::map<std::string, AttributeValue> resultMap;
        for (const auto &[fst, snd]: attributeValue) {
            AttributeValue result;
            result.stringValue = snd.stringValue;
            result.stringSetValue = snd.stringSetValue;
            result.numberValue = snd.numberValue;
            result.numberSetValue = snd.numberSetValue;
            result.boolValue = snd.boolValue;
            result.nullValue = snd.nullValue;
            if (!snd.stringValue.empty()) {
                result.type = "S";
            } else if (!snd.numberValue.empty()) {
                result.type = "N";
            } else if (!snd.stringSetValue.empty()) {
                result.type = "SS";
            } else if (!snd.numberSetValue.empty()) {
                result.type = "NS";
            } else if (!snd.boolValue) {
                result.type = "BOOL";
            } else if (!snd.nullValue) {
                result.type = "NULL";
            }
            resultMap[fst] = result;
        }
        return resultMap;
    }

    // Attribute Value entity -> DTO
    AttributeValue Mapper::map(const Database::Entity::DynamoDb::AttributeValue &attributeValueEntity) {
        AttributeValue attributeValue;
        attributeValue.stringValue = attributeValueEntity.stringValue;
        attributeValue.stringSetValue = attributeValueEntity.stringSetValue;
        attributeValue.numberValue = attributeValueEntity.numberValue;
        attributeValue.numberSetValue = attributeValueEntity.numberSetValue;
        attributeValue.nullValue = attributeValueEntity.nullValue;
        attributeValue.boolValue = attributeValueEntity.boolValue;
        if (!attributeValue.stringValue.empty()) {
            attributeValue.type = "S";
        } else if (!attributeValue.numberValue.empty()) {
            attributeValue.type = "N";
        } else if (!attributeValue.stringSetValue.empty()) {
            attributeValue.type = "SS";
        } else if (!attributeValue.numberSetValue.empty()) {
            attributeValue.type = "NS";
        } else if (!attributeValue.boolValue) {
            attributeValue.type = "BOOL";
        } else if (!attributeValue.nullValue) {
            attributeValue.type = "NULL";
        }
        return attributeValue;
    }

    // Attribute Value entity -> DTO
    Database::Entity::DynamoDb::AttributeValue Mapper::map(const AttributeValue &attributeValueDto) {
        Database::Entity::DynamoDb::AttributeValue attributeValueEntity;
        attributeValueEntity.stringValue = attributeValueDto.stringValue;
        attributeValueEntity.stringSetValue = attributeValueDto.stringSetValue;
        attributeValueEntity.numberValue = attributeValueDto.numberValue;
        attributeValueEntity.numberSetValue = attributeValueDto.numberSetValue;
        attributeValueEntity.nullValue = attributeValueDto.nullValue;
        attributeValueEntity.boolValue = attributeValueDto.boolValue;
        return attributeValueEntity;
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

    // Attribute Value item -> DTO
    ItemCounter Mapper::mapCounter(const Database::Entity::DynamoDb::Item &itemEntity) {
        ItemCounter itemCounterDto;
        itemCounterDto.id = itemEntity.oid;
        itemCounterDto.tableName = itemEntity.tableName;
        itemCounterDto.size = itemEntity.size;
        itemCounterDto.created = itemEntity.created;
        itemCounterDto.modified = itemEntity.modified;
        for (const auto &[fst, snd]: itemEntity.attributes) {
            itemCounterDto.attributes[fst] = map(snd);
        }
        for (const auto &[fst, snd]: itemEntity.keys) {
            itemCounterDto.keys[fst] = map(snd);
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
}// namespace AwsMock::Dto::DynamoDb