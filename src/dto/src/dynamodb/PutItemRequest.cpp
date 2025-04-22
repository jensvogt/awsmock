//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/PutItemRequest.h>

namespace AwsMock::Dto::DynamoDb {

    void PutItemRequest::FromJson(const std::string &jsonString) {

        // Save original body
        body = jsonString;

        try {

            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            tableName = Core::Bson::BsonUtils::GetStringValue(document, "TableName");

            if (document.find("Item") != document.end()) {
                for (const view attributesView = document.view()["Item"].get_document().value; const bsoncxx::document::element &attribute: attributesView) {
                    std::string key = bsoncxx::string::to_string(attribute.key());
                    AttributeValue value;
                    value.FromDocument(attributesView[key].get_document().value);
                    attributes.emplace(key, value);
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string PutItemRequest::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "TableName", tableName);

            if (!attributes.empty()) {
                document attributeObject;
                for (const auto &[fst, snd]: attributes) {
                    document attributeDoc;
                    if (!snd.stringValue.empty()) {
                        attributeDoc.append(kvp("S", snd.stringValue));
                    } else if (!snd.numberValue.empty()) {
                        attributeDoc.append(kvp("N", snd.stringValue));
                    }
                    attributeObject.append(kvp(fst, attributeDoc));
                }
                rootDocument.append(kvp("Item", attributeObject));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
