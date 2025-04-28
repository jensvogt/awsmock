//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/RespondToAuthChallengeResponse.h>

namespace AwsMock::Dto::Cognito {

    std::string RespondToAuthChallengeResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "Session", session);
            Core::Bson::BsonUtils::SetStringValue(document, "ChallengeName", ChallengeNameToString(challengeName));

            document.append(kvp("AuthenticationResult", authenticationResult.ToDocument()));

            if (!challengeParameters.empty()) {
                array jsonArray;
                for (const auto &[fst, snd]: challengeParameters) {
                    jsonArray.append(fst, snd);
                }
                document.append(kvp("ChallengeParameters", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito