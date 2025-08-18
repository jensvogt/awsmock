//
// Created by vogje01 on 06/06/2023.
//

#include "awsmock/dto/docker/model/Port.h"

namespace AwsMock::Dto::Docker {

    void Port::FromDocument(const view_or_value<view, value> &object) {

        try {

            hostIp = Core::Bson::BsonUtils::GetStringValue(object, "HostIp");
            hostPort = Core::Bson::BsonUtils::GetIntValue(object, "HostPort");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string Port::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetIntValue(document, "HostPort", hostPort);
            Core::Bson::BsonUtils::SetStringValue(document, "HostIp", hostIp);
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string Port::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Port &p) {
        os << "Port=" << p.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Docker
