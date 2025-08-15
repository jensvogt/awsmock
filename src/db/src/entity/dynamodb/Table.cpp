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
                    tagsDoc.append(kvp(t.at("Key"), t.at("Value")));
                }
                tableDoc.append(kvp("tags", tagsDoc));
            }

            // Attributes
            if (!attributes.empty()) {
                auto attributesDoc = document{};
                for (const auto &k: attributes) {
                    attributesDoc.append(kvp(k.at("AttributeName"), k.at("AttributeType")));
                }
                tableDoc.append(kvp("attributes", attributesDoc));
            }

            // Key schemas
            if (!keySchemas.empty()) {
                auto keySchemaDoc = document{};
                for (const auto &k: keySchemas) {
                    keySchemaDoc.append(kvp(k.at("AttributeName"), k.at("KeyType")));
                }
                tableDoc.append(kvp("keySchemas", keySchemaDoc));
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
        if (mResult.value().find("attributes") != mResult.value().end()) {
            attributes.clear();
            for (const view tagsView = mResult.value()["attributes"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
                std::map<std::string, std::string> attribute;
                attribute["AttributeName"] = bsoncxx::string::to_string(tagElement.key());
                attribute["AttributeType"] = bsoncxx::string::to_string(tagElement.get_string().value);
                attributes.emplace_back(attribute);
            }
        }

        // Key schemas
        if (mResult.value().find("keySchemas") != mResult.value().end()) {
            keySchemas.clear();
            for (const view keySchemaView = mResult.value()["keySchemas"].get_document().value; const bsoncxx::document::element &keySchemaElement: keySchemaView) {
                std::map<std::string, std::string> key;
                key["AttributeName"] = bsoncxx::string::to_string(keySchemaElement.key());
                key["KeyType"] = bsoncxx::string::to_string(keySchemaElement.get_string().value);
                keySchemas.emplace_back(key);
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