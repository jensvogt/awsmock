//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/InitiateAuthResponse.h>

namespace AwsMock::Dto::Cognito {

    std::string InitiateAuthResponse::ToJson() {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "Session", session);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "ChallengeName", challengeName);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "ClientId", clientId);

            //rootDocument.append(kvp("AuthenticationResult", authenticationResult.ToDocument()));

            // Challenge parameter
            if (!challengeParameters.empty()) {
                document jsonObject;
                for (const auto &[fst, snd]: challengeParameters) {
                    jsonObject.append(kvp(fst, snd));
                }
                rootDocument.append(kvp("ChallengeParameters", jsonObject));
            }

            // Available challenges
            if (!availableChallenges.empty()) {
                array availableChallengesJson;
                for (const auto &challenge: availableChallenges) {
                    availableChallengesJson.append(challenge);
                }
                rootDocument.append(kvp("AvailableChallenges", availableChallengesJson));
            }

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito