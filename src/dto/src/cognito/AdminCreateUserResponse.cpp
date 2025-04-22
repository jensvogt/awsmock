//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/AdminCreateUserResponse.h>

namespace AwsMock::Dto::Cognito {

    void AdminCreateUserResponse::FromJson(const std::string &payload) {

        try {

            const value document = bsoncxx::from_json(payload);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            userName = Core::Bson::BsonUtils::GetStringValue(document, "Username");

            // User attributes
            if (document.find("UserAttributes") != document.end()) {
                array array;
                for (const auto &element: document["UserAttributes"].get_array().value) {
                    UserAttribute userAttribute;
                    userAttribute.FromDocument(element.get_document());
                    userAttributes.emplace_back(userAttribute);
                }
            }
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string AdminCreateUserResponse::ToJson() const {

        try {
            document userDocument;
            Core::Bson::BsonUtils::SetStringValue(userDocument, "Username", userName);
            Core::Bson::BsonUtils::SetBoolValue(userDocument, "Enabled", enabled);

            // User attributes
            if (!userAttributes.empty()) {
                array attributesJsonArray;
                for (const auto &[name, attributeValue]: userAttributes) {
                    document attributeObject;
                    attributeObject.append(kvp("Name", name));
                    attributeObject.append(kvp("Value", attributeValue));
                    attributesJsonArray.append(attributeObject);
                }
                userDocument.append(kvp("Attributes", attributesJsonArray));
            }

            document rootDocument;
            rootDocument.append(kvp("User", userDocument));

            return Core::Bson ::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito