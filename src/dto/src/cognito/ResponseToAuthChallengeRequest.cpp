//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/RespondToAuthChallengeRequest.h>

namespace AwsMock::Dto::Cognito {

    std::string RespondToAuthChallengeRequest::GetUserName() {
        return challengeResponses["USERNAME"];
    }

    std::string RespondToAuthChallengeRequest::GetPasswordClaimSecretBlock() {
        return challengeResponses["PASSWORD_CLAIM_SECRET_BLOCK"];
    }

    std::string RespondToAuthChallengeRequest::GetPasswordClaim_Signature() {
        return challengeResponses["PASSWORD_CLAIM_SIGNATURE"];
    }


    /*{
    "ChallengeName" : "PASSWORD_VERIFIER",
    "ClientId" : "9Oax8ZaPb34CI3byoybhwH2qeo",
    "ChallengeResponses" : {
    "USERNAME" : "jensvogt47@gmail.com",
    "PASSWORD_CLAIM_SECRET_BLOCK" : "AA6WJfsc0ADuUZBFhCfwp4j38e9p7RlxBPWOYP1D",
    "TIMESTAMP" : "Mon Dec 30 13:55:03 UTC 2024",
    "PASSWORD_CLAIM_SIGNATURE" : "zlK9enx6JQuri+EtOvi4OQi2ev8Cb/iAdZwiiydV7Zs="
    },
    "Session" : "f098ee20-4af4-432a-a9fb-9f6550090188",
    "ClientMetadata" : { }
    }*/
    void RespondToAuthChallengeRequest::FromJson(const std::string &jsonString) {

        try {

            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Region");
            clientId = Core::Bson::BsonUtils::GetStringValue(rootDocument, "ClientId");
            session = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Session");
            challengeName = ChallengeNameFromString(Core::Bson::BsonUtils::GetStringValue(rootDocument, "ChallengeName"));

            // Challenge responses
            if (rootDocument.view().find("ChallengeResponses") != rootDocument.view().end()) {
                for (const view jsonObject = rootDocument.view()["ChallengeResponses"].get_document().value; const auto &element: jsonObject) {
                    std::string key = bsoncxx::string::to_string(element.key());
                    const std::string value = bsoncxx::string::to_string(jsonObject[key].get_string().value);
                    challengeResponses[key] = value;
                }
            }

            // Client metadata
            if (rootDocument.view().find("ClientMetadata") != rootDocument.view().end()) {
                for (const view jsonObject = rootDocument.view()["ClientMetadata"].get_document().value; const auto &element: jsonObject) {
                    std::string key = bsoncxx::string::to_string(element.key());
                    const std::string value = bsoncxx::string::to_string(jsonObject[key].get_string().value);
                    clientMetaData[key] = value;
                }
            }
        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string RespondToAuthChallengeRequest::ToJson() {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "ClientId", clientId);

            // Challenge responses
            if (!challengeResponses.empty()) {
                array jsonArray;
                for (const auto &[fst, snd]: challengeResponses) {
                    document jsonObject;
                    jsonObject.append(kvp(fst, snd));
                    jsonArray.append(jsonObject);
                }
                rootDocument.append(kvp("ChallengeResponses", jsonArray));
            }

            // Challenge responses
            if (!clientMetaData.empty()) {
                array jsonArray;
                for (const auto &[fst, snd]: clientMetaData) {
                    document jsonObject;
                    jsonObject.append(kvp(fst, snd));
                    jsonArray.append(jsonObject);
                }
                rootDocument.append(kvp("ClientMetadata", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito