//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/UpdateUserPoolRequest.h>

namespace AwsMock::Dto::Cognito {

    void UpdateUserPoolRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            userPoolId = Core::Bson::BsonUtils::GetStringValue(document, "UserPoolId");

            // Tags
            if (document.view().find("Tags") != document.view().end()) {
                for (const bsoncxx::array::view jsonArray = document.view()["Tags"].get_array().value; const auto &tag: jsonArray) {
                    std::string key = bsoncxx::string::to_string(tag.key());
                    const std::string value = bsoncxx::string::to_string(tag[key].get_string().value);
                    tags[key] = value;
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string UpdateUserPoolRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "UserPoolId", userPoolId);

            // Tags
            if (!tags.empty()) {
                array tagsArray;
                for (const auto &[fst, snd]: tags) {
                    bsoncxx::builder::basic::document tagDocument;
                    Core::Bson::BsonUtils::SetStringValue(tagDocument, "Key", fst);
                    Core::Bson::BsonUtils::SetStringValue(tagDocument, "Value", snd);
                    tagsArray.append(tagDocument);
                }
                document.append(kvp("Tags", tagsArray));
            }

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito