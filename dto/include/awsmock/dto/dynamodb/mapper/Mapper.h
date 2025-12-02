//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_DYNAMODB_MAPPER_H
#define AWSMOCK_DTO_DYNAMODB_MAPPER_H

// AwsMock includes
#include <awsmock/dto/dynamodb/DeleteItemRequest.h>
#include <awsmock/dto/dynamodb/DescribeTableResponse.h>
#include <awsmock/dto/dynamodb/PutItemRequest.h>
#include <awsmock/dto/dynamodb/model/Item.h>
#include <awsmock/dto/dynamodb/model/ItemCounter.h>
#include <awsmock/entity/dynamodb/AttributeValue.h>
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
         * @param table table entity
         * @return Dynamodb entity to be deleted
         * @see Dto::DynamoDb::PutItemRequest
         * @see Database::Entity::DynamoDb::Item
         */
        static Database::Entity::DynamoDb::Item map(const PutItemRequest &request, const Database::Entity::DynamoDb::Table &table);

        /**
         * @brief Maps a DynamoDB PutItemRequest to an entity, which can be saved in the database.
         *
         * @param attributeValue attribute value entity
         * @return Dynamodb attribute DTO
         * @see Dto::DynamoDb::AttributeValue
         * @see Database::Entity::DynamoDb::AttributeValue
         */
        static std::map<std::string, AttributeValue> map(const std::map<std::string, Database::Entity::DynamoDb::AttributeValue> &attributeValue);

        /**
         * @brief Maps a DynamoDB item attribute entity to an item attribute DTO.
         *
         * @param attributeValueEntity DynamoDb item entity
         * @return Dynamodb item attribute DTO
         * @see Dto::DynamoDb::AttributeValue
         * @see Database::Entity::DynamoDb::AttributeValue
         */
        static AttributeValue map(const Database::Entity::DynamoDb::AttributeValue &attributeValueEntity);

        /**
         * @brief Maps a DynamoDB item attribute DTO to an item attribute entity.
         *
         * @param attributeValueDto attribute value DTO
         * @return attribute value entity
         */
        static Database::Entity::DynamoDb::AttributeValue map(const AttributeValue &attributeValueDto);

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
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_S3_MAPPER_H
