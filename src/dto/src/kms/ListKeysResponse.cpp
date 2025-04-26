//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/ListKeysResponse.h>

namespace AwsMock::Dto::KMS {

    void ListKeysResponse::FromJson(const std::string &jsonString) {

        try {

            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Region");
            nextMarker = Core::Bson::BsonUtils::GetStringValue(rootDocument, "NextMarker");
            truncated = Core::Bson::BsonUtils::GetBoolValue(rootDocument, "Truncated");

            // Grant tokens
            if (rootDocument.view().find("Keys") != rootDocument.view().end()) {
                for (const bsoncxx::array::view jsonTokenArray = rootDocument.view()["Keys"].get_array().value; const auto &element: jsonTokenArray) {
                    ListKey key;
                    view jsonObject = element.get_document().value;
                    key.keyId = Core::Bson::BsonUtils::GetStringValue(jsonObject, "KeyId");
                    key.keyArn = Core::Bson::BsonUtils::GetStringValue(jsonObject, "KeyArn");
                    keys.emplace_back(key);
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListKeysResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "NextMarker", nextMarker);
            Core::Bson::BsonUtils::SetBoolValue(document, "Truncated", truncated);

            if (!keys.empty()) {
                array jsonArray;
                for (const auto &element: keys) {
                    jsonArray.append(element.ToDocument());
                }
                document.append(kvp("Keys", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::KMS
