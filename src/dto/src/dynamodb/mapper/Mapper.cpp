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
}// namespace AwsMock::Dto::DynamoDb