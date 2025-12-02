//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/transfer/CreateUserResponse.h>

namespace AwsMock::Dto::Transfer {

    std::string CreateUserResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "ServerId", serverId);
            Core::Bson::BsonUtils::SetStringValue(document, "UserName", userName);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateUserResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateUserResponse &r) {
        os << "CreateUserResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
