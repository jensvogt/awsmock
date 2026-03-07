//
// Created by vogje01 on 03/09/2023.
//

#include "awsmock/entity/cognito/UserPool.h"


#include <awsmock/entity/dynamodb/Table.h>

namespace AwsMock::Database::Entity::DynamoDb {

    view_or_value<view, value> Table::ToDocument() const {

        try {

            document tableDoc;
            tableDoc.append(
                    kvp("region", region),
                    kvp("name", name),
                    kvp("arn", arn),
                    kvp("status", status),
                    kvp("size", bsoncxx::types::b_int64(size)),
                    kvp("itemCount", bsoncxx::types::b_int64(itemCount)),
                    kvp("created", bsoncxx::types::b_date(created)),
                    kvp("modified", bsoncxx::types::b_date(modified)));

            // Tags
            if (!tags.empty()) {
                auto tagsDoc = document{};
                for (const auto &t: tags) {
                    tagsDoc.append(kvp(t.at("key"), t.at("value")));
                }
                tableDoc.append(kvp("tags", tagsDoc));
            }

            // Attributes
            if (!attributeDefinitions.empty()) {
                auto attributesArray = array{};
                for (const auto &attributeDefinition: attributeDefinitions) {
                    attributesArray.append(attributeDefinition.ToDocument());
                }
                tableDoc.append(kvp("attributeDefinitions", attributesArray));
            }

            // Key schemas
            if (!keySchema.empty()) {
                auto keySchemaArray = array{};
                for (const auto &attribute: keySchema) {
                    keySchemaArray.append(attribute.ToDocument());
                }
                tableDoc.append(kvp("keySchema", keySchemaArray));
            }

            // Provisioned throughput
            tableDoc.append(kvp("provisionedThroughput", provisionedThroughput.ToDocument()));

            // Stream specification
            tableDoc.append(kvp("streamSpecification", streamSpecification.ToDocument()));

            return tableDoc.extract();

        } catch (std::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }

    void Table::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        arn = Core::Bson::BsonUtils::GetStringValue(mResult, "arn");
        status = Core::Bson::BsonUtils::GetStringValue(mResult, "status");
        size = Core::Bson::BsonUtils::GetLongValue(mResult, "size");
        itemCount = Core::Bson::BsonUtils::GetLongValue(mResult, "itemCount");
        status = Core::Bson::BsonUtils::GetStringValue(mResult, "status");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        // Get tags
        if (mResult.value().find("tags") != mResult.value().end()) {
            tags.clear();
            for (const view tagsView = mResult.value()["tags"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
                std::map<std::string, std::string> tag;
                tag["Key"] = bsoncxx::string::to_string(tagElement.key());
                tag["Value"] = bsoncxx::string::to_string(tagElement.get_string().value);
                tags.emplace_back(tag);
            }
        }

        // Get attributes
        if (mResult.value().find("attributeDefinitions") != mResult.value().end()) {
            attributeDefinitions.clear();
            for (const view attributesView = mResult.value()["attributeDefinitions"].get_array().value; const bsoncxx::document::element &attributeElement: attributesView) {
                AttributeDefinition attributeDefinition;
                attributeDefinition.FromDocument(attributeElement.get_document().value);
                attributeDefinitions.emplace_back(attributeDefinition);
            }
        }

        // Key schemas
        if (mResult.value().find("keySchema") != mResult.value().end()) {
            keySchema.clear();
            for (const view keySchemaView = mResult.value()["keySchema"].get_array().value; const bsoncxx::document::element &keyElement: keySchemaView) {
                KeySchema keySchemas;
                keySchemas.FromDocument(keyElement.get_document().value);
                keySchema.emplace_back(keySchemas);
            }
        }

        // Provisioned throughput
        if (mResult.value().find("provisionedThroughput") != mResult.value().end()) {
            provisionedThroughput.FromDocument(mResult.value()["provisionedThroughput"].get_document().value);
        }

        // Stream specification
        if (mResult.value().find("streamSpecification") != mResult.value().end()) {
            streamSpecification.FromDocument(mResult.value()["streamSpecification"].get_document().value);
        }
    }

}// namespace AwsMock::Database::Entity::DynamoDb