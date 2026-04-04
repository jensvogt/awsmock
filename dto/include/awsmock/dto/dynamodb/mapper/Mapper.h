//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_DYNAMODB_MAPPER_H
#define AWSMOCK_DTO_DYNAMODB_MAPPER_H

// C++ includes
#include <ranges>

// AwsMock includes
#include <awsmock/dto/dynamodb/DeleteItemRequest.h>
#include <awsmock/dto/dynamodb/DescribeTableResponse.h>
#include <awsmock/dto/dynamodb/PutItemRequest.h>
#include <awsmock/dto/dynamodb/model/Item.h>
#include <awsmock/dto/dynamodb/model/ItemCounter.h>
#include <awsmock/entity/dynamodb/Item.h>
#include <awsmock/entity/dynamodb/Table.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief Maps an entity to the corresponding DTO and vice versa.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class Mapper {

      public:

        /**
         * @brief Maps a DynamoDB table DTO to an entity
         *
         * @param response response struct
         * @return Dynamodb entity
         * @see Dto::DynamoDb::DescribeTableResponse
         * @see Database::Entity::DynamoDb::Table
         */
        static Database::Entity::DynamoDb::Table map(const DescribeTableResponse &response);

        /**
         * @brief Maps a DynamoDB DeleteItemRequest to an entity, which can be deleted in the database.
         *
         * @param request request struct
         * @return Dynamodb entity to be deleted
         * @see Dto::DynamoDb::DeleteItemRequest
         * @see Database::Entity::DynamoDb::Item
         */
        static Database::Entity::DynamoDb::Item map(const DeleteItemRequest &request);

        /**
         * @brief Maps a DynamoDB PutItemRequest to an entity, which can be saved in the database.
         *
         * @param request request struct
         * @return Dynamodb entity to be deleted
         * @see Dto::DynamoDb::PutItemRequest
         * @see Database::Entity::DynamoDb::Item
         */
        static Database::Entity::DynamoDb::Item map(const PutItemRequest &request);

        /**
         * @brief Maps a DynamoDB PutItemRequest to an entity, which can be saved in the database.
         *
         * @param attributes attribute value entities
         * @return Dynamodb attribute DTO
         * @see Dto::DynamoDb::AttributeValue
         * @see Database::Entity::DynamoDb::AttributeValue
         */
        static std::unordered_map<std::string, AttributeValue> map(const std::unordered_map<std::string, Database::Entity::DynamoDb::DynamoValue> &attributes);

        /**
         * @brief Maps a DynamoDB item attribute entity to an item attribute DTO.
         *
         * @param attributeValueEntity DynamoDb item entity
         * @return Dynamodb item attribute DTO
         * @see Dto::DynamoDb::AttributeValue
         * @see Database::Entity::DynamoDb::AttributeValue
         */
        static AttributeValue map(const Database::Entity::DynamoDb::DynamoValue &attributeValueEntity);

        /**
         * @brief Maps a DynamoDB item attribute DTO to an item attribute entity.
         *
         * @param attributeValueDto attribute value DTO
         * @return attribute value entity
         */
        static Database::Entity::DynamoDb::DynamoValue map(const AttributeValue &attributeValueDto);
        static AttributeValue map(const Database::Entity::DynamoDb::KeyValue &keyValue);

        /**
         * @brief Maps a DynamoDB item entity to an item DTO.
         *
         * @param itemEntity DynamoDb item entity
         * @return Dynamodb item DTO
         * @see Dto::DynamoDb::Item
         * @see Database::Entity::DynamoDb::Item
         */
        static Item map(const Database::Entity::DynamoDb::Item &itemEntity);

        /**
         * @brief Maps a DynamoDB item DTO to an entity
         *
         * @param itemDto item DTO
         * @return item entity.
         */
        static Database::Entity::DynamoDb::Item map(const Item &itemDto);

        /**
         * @brief Maps a list of DynamoDB item entities to an item DTO list.
         *
         * @param itemEntities DynamoDb item entity list
         * @return Dynamodb item DTO list
         * @see Dto::DynamoDb::Item
         * @see Database::Entity::DynamoDb::Item
         */
        static std::vector<Item> map(const std::vector<Database::Entity::DynamoDb::Item> &itemEntities);

        /**
         * @brief Maps a table key schema DTO to a key schema entity
         *
         * @param keySchemaDto DynamoDb table key schema DTO
         * @return Dynamodb table key schema entity
         * @see Dto::DynamoDb::KeySchema
         * @see Database::Entity::DynamoDb::KeySchema
         */
        static Database::Entity::DynamoDb::KeySchema map(const KeySchema &keySchemaDto);
        static KeySchema map(const Database::Entity::DynamoDb::KeySchema &keySchemaEntity);

        /**
         * @brief Maps a list of key schema entities to a list of key schema DTOs
         *
         * @param keySchemaEntities list of key schema entities
         * @return list of key schema DTOs
         * @see Dto::DynamoDb::KeySchema
         * @see Database::Entity::DynamoDb::KeySchema
         */
        static std::vector<KeySchema> map(const std::vector<Database::Entity::DynamoDb::KeySchema> &keySchemaEntities);

        /**
         * @brief Maps a table attribute DTO to an attribute entity
         *
         * @param attributeDefinitionDto DynamoDb table attribute
         * @return Dynamodb table attribute entity
         * @see Dto::DynamoDb::KeySchema
         * @see Database::Entity::DynamoDb::KeySchema
         */
        static Database::Entity::DynamoDb::AttributeDefinition map(const AttributeDefinition &attributeDefinitionDto);

        /**
         * @brief Maps a Dynamodb attribute definition entity to a attribute definition DTO
         *
         * @param attributeDefinitionEntity
         * @return attribute definition DTO
         */
        static AttributeDefinition map(const Database::Entity::DynamoDb::AttributeDefinition &attributeDefinitionEntity);

        /**
         * @brief Maps a list Dynamodb attribute definition entity to a list of attribute definition DTOs
         *
         * @param attributeDefinitions list of attribute definition entities
         * @return list of attribute definition DTOs
         */
        static std::vector<AttributeDefinition> map(const std::vector<Database::Entity::DynamoDb::AttributeDefinition> &attributeDefinitions);

        static std::map<std::string, Database::Entity::DynamoDb::DynamoValue> map(const std::map<std::string, AttributeValue> &keyDtos);

        /**
         * @brief Maps a DynamoDB item entity to an item counter DTO.
         *
         * @param itemEntity DynamoDb item entity
         * @return Dynamodb item counter DTO
         * @see Dto::DynamoDb::ItemCounter
         * @see Database::Entity::DynamoDb::Item
         */
        static ItemCounter mapCounter(const Database::Entity::DynamoDb::Item &itemEntity);

        /**
         * @brief Maps a DynamoDB item entity list to an item counter DTO list.
         *
         * @param itemEntities DynamoDb item entity list
         * @return Dynamodb item counter DTO list
         * @see Dto::DynamoDb::ItemCounter
         * @see Database::Entity::DynamoDb::Item
         */
        static std::vector<ItemCounter> mapCounter(const std::vector<Database::Entity::DynamoDb::Item> &itemEntities);
        static std::string DoubleToString(double val);
        static std::string KeyValueToString(const KeyValue &value);
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_S3_MAPPER_H
