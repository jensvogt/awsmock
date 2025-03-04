//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/dynamodb/Table.h>

namespace AwsMock::Database::Entity::DynamoDb {

    view_or_value<view, value> Table::ToDocument() const {

        try {

            document tableDoc;
            tableDoc.append(
                    kvp("region", region),
                    kvp("name", name),
                    kvp("status", status),
                    kvp("size", bsoncxx::types::b_int64(size)),
                    kvp("itemCount", bsoncxx::types::b_int64(itemCount)),
                    kvp("created", bsoncxx::types::b_date(created)),
                    kvp("modified", bsoncxx::types::b_date(modified)));

            // Tags
            if (!tags.empty()) {
                auto tagsDoc = document{};
                for (const auto &[fst, snd]: tags) {
                    tagsDoc.append(kvp(fst, snd));
                }
                tableDoc.append(kvp("tags", tagsDoc));
            }

            // Attributes
            if (!attributes.empty()) {
                auto attributesDoc = document{};
                for (const auto &[fst, snd]: attributes) {
                    attributesDoc.append(kvp(fst, snd));
                }
                tableDoc.append(kvp("attributes", attributesDoc));
            }

            // Key schemas
            if (!keySchemas.empty()) {
                auto keySchemaDoc = document{};
                for (const auto &[fst, snd]: keySchemas) {
                    keySchemaDoc.append(kvp(fst, snd));
                }
                tableDoc.append(kvp("keySchemas", keySchemaDoc));
            }

            // Provisioned throughput
            tableDoc.append(kvp("provisionedThroughput", provisionedThroughput.ToDocument()));

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
        status = Core::Bson::BsonUtils::GetStringValue(mResult, "status");
        size = Core::Bson::BsonUtils::GetLongValue(mResult, "size");
        itemCount = Core::Bson::BsonUtils::GetLongValue(mResult, "itemCount");
        status = Core::Bson::BsonUtils::GetStringValue(mResult, "status");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        // Get tags
        tags = Core::Bson::MapFromBsonObject(mResult, "Tags");
        /*        if (mResult.value().find("tags") != mResult.value().end()) {
            for (const view tagsView = mResult.value()["tags"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
                std::string key = bsoncxx::string::to_string(tagElement.key());
                std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                tags.emplace(key, value);
            }
        }*/

        // Get attributes
        if (mResult.value().find("attributes") != mResult.value().end()) {
            for (const view tagsView = mResult.value()["attributes"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
                std::string key = bsoncxx::string::to_string(tagElement.key());
                std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                attributes.emplace(key, value);
            }
        }

        // Key schemas
        if (mResult.value().find("keySchemas") != mResult.value().end()) {
            for (const view keySchemaView = mResult.value()["keySchemas"].get_document().value; const bsoncxx::document::element &keySchemaElement: keySchemaView) {
                std::string key = bsoncxx::string::to_string(keySchemaElement.key());
                std::string value = bsoncxx::string::to_string(keySchemaView[key].get_string().value);
                keySchemas.emplace(key, value);
            }
        }

        // Provisioned throughput
        if (mResult.value().find("provisionedThroughput") != mResult.value().end()) {
            provisionedThroughput.FromDocument(mResult.value()["provisionedThroughput"].get_document().value);
        }
    }

    std::string Table::ToJson() const {
        return to_json(ToDocument());
    }

    std::string Table::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Table &d) {
        os << "Table=" << d.ToJson();
        return os;
    }
}// namespace AwsMock::Database::Entity::DynamoDb