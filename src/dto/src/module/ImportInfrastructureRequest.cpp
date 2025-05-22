//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/module/ImportInfrastructureRequest.h>

namespace AwsMock::Dto::Module {

    void ImportInfrastructureRequest::FromJson(const std::string &payload) {

        try {

            const value documentValue = bsoncxx::from_json(payload);
            includeObjects = Core::Bson::BsonUtils::GetBoolValue(documentValue, "includeObjects");
            cleanFirst = Core::Bson::BsonUtils::GetBoolValue(documentValue, "cleanFirst");

            // Infrastructure object
            if (documentValue.find("infrastructure") != documentValue.end()) {
                infrastructure.FromDocument(documentValue.view()["infrastructure"].get_document().value);
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ImportInfrastructureRequest::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetBoolValue(rootDocument, "includeObjects", includeObjects);
            Core::Bson::BsonUtils::SetBoolValue(rootDocument, "cleanFirst", cleanFirst);
            Core::Bson::BsonUtils::SetDocumentValue(rootDocument, "infrastructure", infrastructure.ToDocument().view());
            std::string tp = Core::Bson::BsonUtils::ToJsonString(rootDocument);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Module