//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/InitiateAuthRequest.h>

namespace AwsMock::Dto::Cognito {

    std::string InitiateAuthRequest::GetUserId() {
        return authParameters["USERNAME"];
    }

    std::string InitiateAuthRequest::GetPassword() {
        return authParameters["PASSWORD"];
    }

    std::string InitiateAuthRequest::GetClientSecret() {
        return authParameters["SECRET_HASH"];
    }

    void InitiateAuthRequest::FromJson(const std::string &jsonString) {
        try {
            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "region");
            clientId = Core::Bson::BsonUtils::GetStringValue(rootDocument, "ClientId");
            authFlow = AuthFlowTypeFromString(Core::Bson::BsonUtils::GetStringValue(rootDocument, "AuthFlow"));

            // Auth parameter
            if (Core::Bson::FindBsonObject(rootDocument, "AuthParameters")) {
                authParameters = Core::Bson::MapFromBsonObject(rootDocument, "AuthParameters");
            }

            // Auth parameter
            if (Core::Bson::FindBsonObject(rootDocument, "ClientMetadata")) {
                clientMetaData = Core::Bson::MapFromBsonObject(rootDocument, "ClientMetadata");
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string InitiateAuthRequest::ToJson() {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "ClientId", clientId);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "AuthFlow", AuthFlowTypeToString(authFlow));

            // Auth parameter
            if (!authParameters.empty()) {
                document authFlowObject;
                for (const auto &[fst, snd]: authParameters) {
                    authFlowObject.append(kvp(fst, snd));
                }
                rootDocument.append(kvp("AuthParameter", authFlowObject));
            }

            // Metadata
            if (!clientMetaData.empty()) {
                document metadataObject;
                for (const auto &[fst, snd]: clientMetaData) {
                    metadataObject.append(kvp(fst, snd));
                }
                rootDocument.append(kvp("ClientMetadata", metadataObject));
            }

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito