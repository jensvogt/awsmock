//
// Created by vogje01 on 10/23/23.
//

#include "awsmock/dto/module/model/GatewayConfig.h"

namespace AwsMock::Dto::Module {

    std::string GatewayConfig::ToJson() const {
        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "region", region);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "protocol", protocol);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "endpoint", endpoint);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "host", host);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "address", address);
            Core::Bson::BsonUtils::SetIntValue(rootDocument, "port", port);
            Core::Bson::BsonUtils::SetIntValue(rootDocument, "pid", pid);
            Core::Bson::BsonUtils::SetBoolValue(rootDocument, "pretty", prettyPrint);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "accessId", accessId);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "clientId", clientId);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "user", user);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "dataDir", dataDir);
            Core::Bson::BsonUtils::SetBoolValue(rootDocument, "databaseActive", databaseActive);
            Core::Bson::BsonUtils::SetStringValue(rootDocument, "version", version);

            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    GatewayConfig GatewayConfig::FromJson(const std::string &payload) {

        if (payload.empty()) {
            return {};
        }
        const value documentValue = bsoncxx::from_json(payload);

        try {
            GatewayConfig gatewayConfig;
            gatewayConfig.region = Core::Bson::BsonUtils::GetStringValue(documentValue, "region");
            gatewayConfig.protocol = Core::Bson::BsonUtils::GetStringValue(documentValue, "protocol");
            gatewayConfig.endpoint = Core::Bson::BsonUtils::GetStringValue(documentValue, "endpoint");
            gatewayConfig.host = Core::Bson::BsonUtils::GetStringValue(documentValue, "host");
            gatewayConfig.port = Core::Bson::BsonUtils::GetIntValue(documentValue, "port");
            gatewayConfig.pid = Core::Bson::BsonUtils::GetIntValue(documentValue, "pid");
            gatewayConfig.prettyPrint = Core::Bson::BsonUtils::GetBoolValue(documentValue, "prettyPrint");
            gatewayConfig.accessId = Core::Bson::BsonUtils::GetStringValue(documentValue, "accessId");
            gatewayConfig.clientId = Core::Bson::BsonUtils::GetStringValue(documentValue, "clientId");
            gatewayConfig.user = Core::Bson::BsonUtils::GetStringValue(documentValue, "user");
            gatewayConfig.dataDir = Core::Bson::BsonUtils::GetStringValue(documentValue, "dataDir");
            gatewayConfig.databaseActive = Core::Bson::BsonUtils::GetBoolValue(documentValue, "databaseActive");
            gatewayConfig.version = Core::Bson::BsonUtils::GetStringValue(documentValue, "version");
            return gatewayConfig;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GatewayConfig::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GatewayConfig &g) {
        os << "GatewayConfig=" << g.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Module
