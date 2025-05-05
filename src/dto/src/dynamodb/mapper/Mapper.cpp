//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/dynamodb/GetItemRequest.h>
#include <awsmock/dto/dynamodb/mapper/Mapper.h>

namespace AwsMock::Dto::DynamoDb {

    Database::Entity::DynamoDb::Table Mapper::map(const DescribeTableResponse &response) {
        Database::Entity::DynamoDb::Table tableEntity;
        tableEntity.region = response.region,
        tableEntity.name = response.tableName,
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

        for (const auto &[fst, snd]: request.key.keys) {
            Database::Entity::DynamoDb::AttributeValue attributeValue;
            attributeValue.stringValue = snd.stringValue;
            attributeValue.stringSetValue = snd.stringSetValue;
            attributeValue.numberValue = snd.numberValue;
            attributeValue.numberSetValue = snd.numberSetValue;
            attributeValue.boolValue = snd.boolValue;
            attributeValue.nullValue = snd.nullValue;
            item.keys[fst] = attributeValue;
        }

        return item;
    }

    Database::Entity::DynamoDb::Item Mapper::map(const PutItemRequest &request, const Database::Entity::DynamoDb::Table &table) {

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

            if (table.keySchemas.contains(fst)) {
                item.keys[fst] = attribute;
            }
        }
        return item;
    }

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
        return attributeValue;
    }

    // Attribute Value item -> DTO
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