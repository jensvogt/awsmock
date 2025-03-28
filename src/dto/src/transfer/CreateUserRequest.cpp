//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/transfer/CreateUserRequest.h>

namespace AwsMock::Dto::Transfer {

    std::string CreateUserRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "ServerId", serverId);
            Core::Bson::BsonUtils::SetStringValue(document, "UserName", userName);
            Core::Bson::BsonUtils::SetStringValue(document, "Password", password);
            Core::Bson::BsonUtils::SetStringValue(document, "HomeDirectory", homeDirectory);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void CreateUserRequest::FromJson(const std::string &jsonString) {

        try {
            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Region");
            serverId = Core::Bson::BsonUtils::GetStringValue(rootDocument, "ServerId");
            userName = Core::Bson::BsonUtils::GetStringValue(rootDocument, "UserName");
            password = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Password");
            homeDirectory = Core::Bson::BsonUtils::GetStringValue(rootDocument, "HomeDirectory");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateUserRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateUserRequest &r) {
        os << "CreateUserRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
