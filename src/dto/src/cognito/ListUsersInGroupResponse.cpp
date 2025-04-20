//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/ListUsersInGroupResponse.h>

namespace AwsMock::Dto::Cognito {

    std::string ListUsersInGroupResponse::ToJson() {

        try {

            document rootDocument;

            if (!users.empty()) {
                array jsonArray;
                for (const auto &user: users) {
                    jsonArray.append(user.ToDocument());
                }
                rootDocument.append(kvp("Users", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito