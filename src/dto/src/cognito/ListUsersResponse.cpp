//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/ListUsersResponse.h>

namespace AwsMock::Dto::Cognito {

    std::string ListUsersResponse::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "Total", total);

            if (!users.empty()) {
                array usersArray;
                for (const auto &user: users) {
                    document userJson;
                    Core::Bson::BsonUtils::SetStringValue(userJson, "Username", user.userName);
                    Core::Bson::BsonUtils::SetBoolValue(userJson, "Enabled", user.enabled);
                    Core::Bson::BsonUtils::SetStringValue(userJson, "UserStatus", UserStatusToString(user.userStatus));
                    Core::Bson::BsonUtils::SetDateValue(userJson, "LastModifiedDate", user.modified);
                    Core::Bson::BsonUtils::SetDateValue(userJson, "CreationDate", user.created);
                    usersArray.append(userJson);
                }
                rootDocument.append(kvp("Users", usersArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito