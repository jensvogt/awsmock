//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/transfer/ListUsersResponse.h>

namespace AwsMock::Dto::Transfer {

    std::string ListUsersResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "ServerId", serverId);
            Core::Bson::BsonUtils::SetStringValue(document, "NextToken", nextToken);

            // Users
            if (users.empty()) {
                array jsonArray;
                for (const auto &user: users) {
                    jsonArray.append(user.ToDocument());
                }
                document.append(kvp("Users", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListUsersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListUsersResponse &r) {
        os << "ListUserResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
