//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/CreateUserPoolRequest.h>

namespace AwsMock::Dto::Cognito {

    void CreateUserPoolRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            name = Core::Bson::BsonUtils::GetStringValue(document, "PoolName");
            domain = Core::Bson::BsonUtils::GetStringValue(document, "Domain");

            // Get tags
            if (document.find("tags") != document.end()) {
                for (const view tagsView = document["tags"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
                    std::string key = bsoncxx::string::to_string(tagElement.key());
                    std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                    tags.emplace(key, value);
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateUserPoolRequest::ToJson() {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);

            if (!tags.empty()) {
                document jsonObject;
                for (const auto &[fst, snd]: tags) {
                    jsonObject.append(kvp(fst, snd));
                }
                rootDocument.append(kvp("Tags", jsonObject));
            }

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito