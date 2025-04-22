//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/ListUserCountersResponse.h>

namespace AwsMock::Dto::Cognito {

    std::string ListUserCountersResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetLongValue(document, "total", total);

            if (!users.empty()) {
                array jsonArray;
                for (const auto &user: users) {
                    jsonArray.append(user.ToDocument());
                }
                document.append(kvp("users", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito