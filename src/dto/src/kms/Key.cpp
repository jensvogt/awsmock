//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/model/Key.h>

namespace AwsMock::Dto::KMS {

    void Key::FromDocument(const view_or_value<view, value> &jsonObject) {

        try {

            keyId = Core::Bson::BsonUtils::GetStringValue(jsonObject, "KeyId");
            keySpec = KeySpecFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "KeySpec"));
            keyUsage = KeyUsageFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "KeyUsage"));
            keyState = KeyStateFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "KeyState"));
            description = Core::Bson::BsonUtils::GetStringValue(jsonObject, "Description");
            arn = Core::Bson::BsonUtils::GetStringValue(jsonObject, "Arn");
            creationDate = Core::Bson::BsonUtils::GetLongValue(jsonObject, "CreationDate");
            multiRegion = Core::Bson::BsonUtils::GetBoolValue(jsonObject, "MultiRegion");
            enabled = Core::Bson::BsonUtils::GetBoolValue(jsonObject, "Enabled");
            origin = OriginFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "Origin"));

            // Grant tokens
            if (jsonObject.view().find("EncryptionAlgorithms") != jsonObject.view().end()) {
                for (const bsoncxx::array::view jsonArray = jsonObject.view()["EncryptionAlgorithms"].get_array().value; const auto &element: jsonArray) {
                    encryptionAlgorithms.emplace_back(EncryptionAlgorithmsFromString(std::string(element.get_string().value)));
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void Key::FromJson(const std::string &jsonString) {

        const value document = bsoncxx::from_json(jsonString);
        FromDocument(document.view());
    }

    view_or_value<view, value> Key::ToDocument() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "KeyId", keyId);
            Core::Bson::BsonUtils::SetStringValue(document, "KeySpec", KeySpecToString(keySpec));
            Core::Bson::BsonUtils::SetStringValue(document, "KeyUsage", KeyUsageToString(keyUsage));
            Core::Bson::BsonUtils::SetStringValue(document, "KeyState", KeyStateToString(keyState));
            Core::Bson::BsonUtils::SetStringValue(document, "Arn", arn);
            Core::Bson::BsonUtils::SetLongValue(document, "CreationDate", creationDate);
            Core::Bson::BsonUtils::SetLongValue(document, "DeletionDate", deletionDate);
            Core::Bson::BsonUtils::SetBoolValue(document, "MultiRegion", multiRegion);
            Core::Bson::BsonUtils::SetStringValue(document, "Origin", OriginToString(origin));

            if (!encryptionAlgorithms.empty()) {
                array jsonArray;
                for (const auto &element: encryptionAlgorithms) {
                    jsonArray.append(EncryptionAlgorithmsToString(element));
                }
                document.append(kvp("EncryptionAlgorithms", jsonArray));
            }
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string Key::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Key::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Key &r) {
        os << "Key=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::KMS
