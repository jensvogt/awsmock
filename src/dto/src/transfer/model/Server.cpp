//
// Created by vogje01 on 12/18/23.
//

#include <awsmock/dto/transfer/model/IdentityProviderDetails.h>
#include <awsmock/dto/transfer/model/Server.h>

namespace AwsMock::Dto::Transfer {

    void Server::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            serverId = Core::Bson::BsonUtils::GetStringValue(document, "ServerId");
            arn = Core::Bson::BsonUtils::GetStringValue(document, "Arn");
            domain = Core::Bson::BsonUtils::GetStringValue(document, "Domain");
            endpointType = Core::Bson::BsonUtils::GetStringValue(document, "EndpointType");
            identityProviderType = IdentityProviderTypeFromString(Core::Bson::BsonUtils::GetStringValue(document, "IdentityProviderType"));
            state = Core::Bson::BsonUtils::GetStringValue(document, "State");
            userCount = Core::Bson::BsonUtils::GetIntValue(document, "UserCount");
            port = Core::Bson::BsonUtils::GetIntValue(document, "Port");
            concurrency = Core::Bson::BsonUtils::GetIntValue(document, "Concurrency");
            lastStarted = Core::Bson::BsonUtils::GetDateValue(document, "LastStarted");
            created = Core::Bson::BsonUtils::GetDateValue(document, "Created");
            modified = Core::Bson::BsonUtils::GetDateValue(document, "Modified");

            if (document.find("IdentityProviderDetails") != document.end()) {
                identityProviderDetails.FromDocument(document.view()["IdentityProviderDetails"].get_document().value);
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    view_or_value<view, value> Server::ToDocument() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "ServerId", serverId);
            Core::Bson::BsonUtils::SetStringValue(document, "Arn", arn);
            Core::Bson::BsonUtils::SetStringValue(document, "Domain", domain);
            Core::Bson::BsonUtils::SetStringValue(document, "IdentityProviderType", IdentityProviderTypeToString(identityProviderType));
            Core::Bson::BsonUtils::SetStringValue(document, "EndpointType", endpointType);
            Core::Bson::BsonUtils::SetStringValue(document, "LoggingRole", loggingRole);
            Core::Bson::BsonUtils::SetStringValue(document, "State", state);
            Core::Bson::BsonUtils::SetIntValue(document, "UserCount", userCount);
            Core::Bson::BsonUtils::SetIntValue(document, "Port", port);
            Core::Bson::BsonUtils::SetIntValue(document, "Concurrency", concurrency);
            Core::Bson::BsonUtils::SetDateValue(document, "LastStarted", lastStarted);
            Core::Bson::BsonUtils::SetDateValue(document, "Created", created);
            Core::Bson::BsonUtils::SetDateValue(document, "Modified", modified);
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string Server::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string Server::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const Server &r) {
        os << "Server=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
