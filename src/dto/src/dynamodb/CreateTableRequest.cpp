//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/CreateTableRequest.h>

namespace AwsMock::Dto::DynamoDb {

    void CreateTableRequest::FromJson(const std::string &jsonBody) {

        if (jsonBody.empty()) {
            return;
        }

        // Save original body
        body = jsonBody;

        try {

            document document;
            const value documentValue = bsoncxx::from_json(body);

            region = Core::Bson::BsonUtils::GetStringValue(documentValue, "Region");
            tableClass = Core::Bson::BsonUtils::GetStringValue(documentValue, "TableClass");
            tableName = Core::Bson::BsonUtils::GetStringValue(documentValue, "TableName");

            // Tags
            if (documentValue.find("Tags") != documentValue.end()) {
                for (const bsoncxx::array::view arrayView{documentValue["Tags"].get_array().value}; const bsoncxx::array::element &tagElement: arrayView) {
                    std::string key = Core::Bson::BsonUtils::GetStringValue(tagElement["Key"]);
                    std::string value = Core::Bson::BsonUtils::GetStringValue(tagElement["Value"]);
                    tags.emplace(key, value);
                }
            }

            // Attributes
            if (documentValue.find("AttributeDefinitions") != documentValue.end()) {
                for (const bsoncxx::array::view arrayView{documentValue["AttributeDefinitions"].get_array().value}; const bsoncxx::array::element &attributeElement: arrayView) {
                    const std::string name = Core::Bson::BsonUtils::GetStringValue(attributeElement["AttributeName"]);
                    const std::string type = Core::Bson::BsonUtils::GetStringValue(attributeElement["AttributeType"]);
                    attributes[name] = type;
                }
            }

            // Key schemas
            if (documentValue.find("KeySchema") != documentValue.end()) {
                for (const bsoncxx::array::view arrayView{documentValue["KeySchema"].get_array().value}; const bsoncxx::array::element &keySchemaElement: arrayView) {
                    const std::string name = Core::Bson::BsonUtils::GetStringValue(keySchemaElement["AttributeName"]);
                    const std::string type = Core::Bson::BsonUtils::GetStringValue(keySchemaElement["KeyType"]);
                    keySchemas[name] = type;
                }
            }

            // Provisioned throughput
            if (documentValue.find("ProvisionedThroughput") != documentValue.end()) {
                provisionedThroughput.FromDocument(documentValue["ProvisionedThroughput"].get_document().view());
            }

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateTableRequest::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "TableName", tableName);
            Core::Bson::BsonUtils::SetStringValue(document, "TableClass", tableClass);
            Core::Bson::BsonUtils::SetDocumentValue(document, "ProvisionedThroughput", provisionedThroughput.ToDocument());

            // Attributes
            if (!attributes.empty()) {
                array attributeArray;
                for (const auto &[fst, snd]: attributes) {
                    bsoncxx::builder::basic::document attributeDocument;
                    Core::Bson::BsonUtils::SetStringValue(attributeDocument, "AttributeName", fst);
                    Core::Bson::BsonUtils::SetStringValue(attributeDocument, "AttributeType", snd);
                    attributeArray.append(attributeDocument);
                }
                document.append(kvp("AttributeDefinitions", attributeArray));
            }

            // Key schema
            if (!keySchemas.empty()) {
                array keySchemasArray;
                for (const auto &[fst, snd]: keySchemas) {
                    bsoncxx::builder::basic::document keySchemaDocument;
                    Core::Bson::BsonUtils::SetStringValue(keySchemaDocument, "AttributeName", fst);
                    Core::Bson::BsonUtils::SetStringValue(keySchemaDocument, "KeyType", snd);
                    keySchemasArray.append(keySchemaDocument);
                }
                document.append(kvp("KeySchema", keySchemasArray));
            }

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

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
