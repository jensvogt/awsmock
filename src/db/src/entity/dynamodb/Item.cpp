//
// Created by vogje01 on 03/09/2023.
//

#include <awsmock/entity/dynamodb/Item.h>
#include <boost/fusion/sequence/intrinsic/size.hpp>

namespace AwsMock::Database::Entity::DynamoDb {

    view_or_value<view, value> Item::ToDocument() const {

        try {

            auto attributesDoc = document{};
            for (const auto &[fst, snd]: attributes) {
                attributesDoc.append(kvp(fst, snd.ToDocument()));
            }

            auto keyDoc = document{};
            for (const auto &[fst, snd]: keys) {
                keyDoc.append(kvp(fst, snd.ToDocument()));
            }

            auto itemDoc = document{};
            Core::Bson::BsonUtils::SetStringValue(itemDoc, "oid", oid);
            Core::Bson::BsonUtils::SetStringValue(itemDoc, "region", region);
            Core::Bson::BsonUtils::SetStringValue(itemDoc, "tableName", tableName);
            Core::Bson::BsonUtils::SetLongValue(itemDoc, "size", size);
            Core::Bson::BsonUtils::SetDateValue(itemDoc, "created", created);
            Core::Bson::BsonUtils::SetDateValue(itemDoc, "modified", modified);
            Core::Bson::BsonUtils::SetDocumentValue(itemDoc, "attributes", attributesDoc);
            Core::Bson::BsonUtils::SetDocumentValue(itemDoc, "keys", keyDoc);
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

            // Attributes
            if (mResult.view().find("attributes") != mResult.view().end()) {
                attributes.clear();
                for (const auto value = mResult.view()["attributes"].get_document().value; auto &v: value) {
                    std::string name = bsoncxx::string::to_string(v.key());
                    AttributeValue attribute;
                    attribute.FromDocument(value[name].get_document().view());
                    attributes.emplace(name, attribute);
                }
            }

            // Keys
            if (mResult.view().find("keys") != mResult.view().end()) {
                keys.clear();
                for (const auto value = mResult.view()["keys"].get_document().value; auto &v: value) {
                    std::string name = bsoncxx::string::to_string(v.key());
                    AttributeValue attribute;
                    attribute.FromDocument(value[name].get_document().view());
                    keys.emplace(name, attribute);
                }
            }
            return *this;

        } catch (const std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    Item Item::FromDynamodb(const view_or_value<view, value> &mResult) {

        try {

            oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
            region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
            tableName = Core::Bson::BsonUtils::GetStringValue(mResult, "tableName");
            created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
            modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

            for (const bsoncxx::document::element &tagElement: mResult.view()) {
                if (std::string key = bsoncxx::string::to_string(tagElement.key()); key != "_id" && key != "region" && key != "tableName") {
                    AttributeValue attribute;
                    attribute.FromDocument(mResult.view()[key].get_document().value);
                    attributes.emplace(key, attribute);
                    size += static_cast<long>(attribute.ToString().size());
                }
            }
            return *this;

        } catch (const std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::DynamoDb