//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/entity/dynamodb/AttributeValue.h>
#include <jwt-cpp/jwt.h>

namespace AwsMock::Database::Entity::DynamoDb {

    view_or_value<view, value> AttributeValue::ToDocument() const {

        auto attributeDoc = document{};
        if (!stringValue.empty()) {
            Core::Bson::BsonUtils::SetStringValue(attributeDoc, "S", stringValue);
        }
        if (!numberValue.empty()) {
            Core::Bson::BsonUtils::SetStringValue(attributeDoc, "N", numberValue);
        }
        if (boolValue) {
            Core::Bson::BsonUtils::SetBoolValue(attributeDoc, "BOOL", *boolValue);
        }
        if (nullValue) {
            Core::Bson::BsonUtils::SetBoolValue(attributeDoc, "nullptr", *nullValue);
        }

        // Convert string set to document
        if (!stringSetValue.empty()) {
            auto stringSetDoc = array{};
            for (const auto &sValue: stringSetValue) {
                stringSetDoc.append(sValue);
            }
            attributeDoc.append(kvp("SS", stringSetDoc));
        }

        // Convert number set to document
        if (!numberSetValue.empty()) {
            auto numberSetDoc = array{};
            for (const auto &nValue: numberSetValue) {
                numberSetDoc.append(nValue);
            }
            attributeDoc.append(kvp("NS", numberSetDoc));
        }
        return attributeDoc.extract();
    }

    void AttributeValue::FromDocument(view_or_value<view, value> mResult) {
        if (mResult.view().find("S") != mResult.view().end()) {
            stringValue = Core::Bson::BsonUtils::GetStringValue(mResult, "S");
        } else if (mResult.view().find("N") != mResult.view().end()) {
            numberValue = Core::Bson::BsonUtils::GetStringValue(mResult, "N");
        } else if (mResult.view().find("BOOL") != mResult.view().end()) {
            boolValue = std::make_shared<bool>(Core::Bson::BsonUtils::GetBoolValue(mResult, "BOOL"));
        } else if (mResult.view().find("nullptr") != mResult.view().end()) {
            nullValue = std::make_shared<bool>(Core::Bson::BsonUtils::GetBoolValue(mResult, "nullptr"));
        }
    }

}// namespace AwsMock::Database::Entity::DynamoDb
