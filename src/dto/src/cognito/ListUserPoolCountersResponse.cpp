//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/ListUserPoolCountersResponse.h>

namespace AwsMock::Dto::Cognito {

    std::string ListUserPoolCountersResponse::ToJson() {

        try {

            document document;
            Core::Bson::BsonUtils::SetLongValue(document, "total", total);

            if (!userPools.empty()) {
                array jsonArray;
                for (const auto &userPool: userPools) {
                    jsonArray.append(userPool.ToDocument());
                }
                document.append(kvp("userPools", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito