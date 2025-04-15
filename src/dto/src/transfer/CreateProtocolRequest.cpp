//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/transfer/CreateProtocolRequest.h>

namespace AwsMock::Dto::Transfer {

    std::string CreateProtocolRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "ServerId", serverId);
            Core::Bson::BsonUtils::SetStringValue(document, "Protocol", ProtocolTypeToString(protocol));
            Core::Bson::BsonUtils::SetIntValue(document, "Port", port);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void CreateProtocolRequest::FromJson(const std::string &jsonString) {

        try {
            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Region");
            serverId = Core::Bson::BsonUtils::GetStringValue(rootDocument, "ServerId");
            protocol = ProtocolTypeFromString(Core::Bson::BsonUtils::GetStringValue(rootDocument, "Protocol"));
            port = Core::Bson::BsonUtils::GetIntValue(rootDocument, "Port");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string CreateProtocolRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateProtocolRequest &r) {
        os << "CreateProtocolRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
