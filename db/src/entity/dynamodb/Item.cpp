//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/dynamodb/Item.h>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

namespace AwsMock::Database::Entity::DynamoDb {

    view_or_value<view, value> Item::ToDocument() const {

        try {

            // Attributes
            auto attributesDoc = document{};
            if (!attributes.empty()) {
                for (const auto &[k, v]: attributes) {
                    attributesDoc.append(kvp(k, v.ToDocument()));
                }
            }

            // Key schemas
            auto keysDoc = document{};
            if (!keys.empty()) {
                for (const auto &[k, v]: keys) {
                    keysDoc.append(kvp(k, v.ToDocument()));
                }
            }

            auto itemDoc = document{};
            Core::Bson::BsonUtils::SetStringValue(itemDoc, "oid", oid);
            Core::Bson::BsonUtils::SetStringValue(itemDoc, "region", region);
            Core::Bson::BsonUtils::SetStringValue(itemDoc, "tableName", tableName);
            Core::Bson::BsonUtils::SetLongValue(itemDoc, "size", size);
            Core::Bson::BsonUtils::SetDocumentValue(itemDoc, "attributes", attributesDoc);
            Core::Bson::BsonUtils::SetDocumentValue(itemDoc, "keys", keysDoc);
            Core::Bson::BsonUtils::SetDateValue(itemDoc, "created", created);
            Core::Bson::BsonUtils::SetDateValue(itemDoc, "modified", modified);
            return itemDoc.extract();

        } catch (const std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Item Item::FromDocument(const view_or_value<view, value> &mResult) {

        try {

            oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
            region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
            tableName = Core::Bson::BsonUtils::GetStringValue(mResult, "tableName");
            size = Core::Bson::BsonUtils::GetLongValue(mResult, "size");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            // Get attributes
            if (mResult.view().find("attributes") != mResult.view().end()) {
                attributes.clear();
                for (const view attributesView = mResult.view()["attributes"].get_document().value; const bsoncxx::document::element &attributeElement: attributesView) {
                    AttributeValue attributeValue;
                    attributeValue.FromDocument(attributeElement.get_document().view());
                    attributes[std::string(attributeElement.key())] = attributeValue;
                }
            }

            // Key schemas
            if (mResult.view().find("keys") != mResult.view().end()) {
                keys.clear();
                for (const view keysView = mResult.view()["keys"].get_document().value; const bsoncxx::document::element &attributeElement: keysView) {
                    AttributeValue attributeValue;
                    attributeValue.FromDocument(attributeElement.get_document().view());
                    keys[std::string(attributeElement.key())] = attributeValue;
                }
            }
            return *this;

        } catch (const std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::DynamoDb