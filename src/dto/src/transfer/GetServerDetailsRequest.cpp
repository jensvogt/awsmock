//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/transfer/GetServerDetailsRequest.h>

namespace AwsMock::Dto::Transfer {

    void GetServerDetailsRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "region");
            serverId = Core::Bson::BsonUtils::GetStringValue(document, "serverId");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    std::string GetServerDetailsRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "serverId", serverId);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    [[nodiscard]] std::string GetServerDetailsRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetServerDetailsRequest &r) {
        os << "GetServerDetailsRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
