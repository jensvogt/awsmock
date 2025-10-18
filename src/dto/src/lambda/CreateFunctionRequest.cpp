//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/lambda/CreateFunctionRequest.h>

namespace AwsMock::Dto::Lambda {

    void CreateFunctionRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            functionName = Core::Bson::BsonUtils::GetStringValue(document, "FunctionName");
            runtime = Core::Bson::BsonUtils::GetStringValue(document, "Runtime");
            role = Core::Bson::BsonUtils::GetStringValue(document, "Role");
            version = Core::Bson::BsonUtils::GetStringValue(document, "Version");
            handler = Core::Bson::BsonUtils::GetStringValue(document, "Handler");
            timeout = Core::Bson::BsonUtils::GetIntValue(document, "Timeout");
            memorySize = Core::Bson::BsonUtils::GetLongValue(document, "MemorySize");

            // Tags
            if (document.view().find("Tags") != document.view().end()) {
                for (const view jsonObject = document.view()["Tags"].get_document().value; const auto &tag: jsonObject) {
                    std::string key = bsoncxx::string::to_string(tag.key());
                    const std::string value = bsoncxx::string::to_string(jsonObject[key].get_string().value);
                    tags[key] = value;
                }
            }

            // EphemeralStorage
            if (document.view().find("EphemeralStorage") != document.view().end()) {
                ephemeralStorage.FromDocument(document.view()["EphemeralStorage"].get_document().value);
            }

            // Environment
            if (document.view().find("Environment") != document.view().end()) {
                environment.FromDocument(document.view()["Environment"].get_document().value);
            }

            // Code
            if (document.view().find("Code") != document.view().end()) {
                code.FromDocument(document.view()["Code"].get_document().value);
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateFunctionRequest::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "User", user);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "FunctionName", functionName);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Runtime", runtime);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Role", role);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Version", version);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Handler", handler);
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "MemorySize", memorySize);
            Core::Bson::BsonUtils::SetIntValue(rootDocument, "Timeout", timeout);

            // Code
            rootDocument.append(kvp("Code", code.ToDocument()));

            // Tags
            if (!tags.empty()) {
                array jsonArray;
                for (const auto &[fst, snd]: tags) {
                    document tagObject;
                    tagObject.append(kvp(fst, snd));
                    jsonArray.append(tagObject);
                }
                rootDocument.append(kvp("Tags", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Lambda
