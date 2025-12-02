//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/transfer/StartServerRequest.h>

namespace AwsMock::Dto::Transfer {

    void StartServerRequest::FromJson(const std::string &jsonString) {

        try {

            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Region");
            serverId = Core::Bson::BsonUtils::GetStringValue(rootDocument, "ServerId");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string StartServerRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "ServerId", serverId);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string StartServerRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const StartServerRequest &r) {
        os << "StartServerRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
