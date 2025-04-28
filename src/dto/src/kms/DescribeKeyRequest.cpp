//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/DescribeKeyRequest.h>

namespace AwsMock::Dto::KMS {

    void DescribeKeyRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            keyId = Core::Bson::BsonUtils::GetStringValue(document, "KeyId");

            // Grant tokens
            if (document.view().find("GrantTokens") != document.view().end()) {
                for (const bsoncxx::array::view jsonTokenArray = document.view()["GrantTokens"].get_array().value; const auto &element: jsonTokenArray) {
                    grantTokens.emplace_back(element.get_string().value);
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string DescribeKeyRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "KeyId", keyId);

            if (!grantTokens.empty()) {
                array jsonArray;
                for (const auto &element: grantTokens) {
                    jsonArray.append(element);
                }
                document.append(kvp("GrantTokens", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::KMS
