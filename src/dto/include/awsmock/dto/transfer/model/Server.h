//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_TRANSFER_SERVER_H
#define AWSMOCK_DTO_TRANSFER_SERVER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/transfer/model/IdentityProviderDetails.h>
#include <awsmock/dto/transfer/model/IdentityProviderType.h>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief Server model
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Server final : Common::BaseCounter<Server> {

        /**
         * ARN
         */
        std::string arn;

        /**
         * Domain
         */
        std::string domain;

        /**
         * Identity provider type
         */
        IdentityProviderType identityProviderType = IdentityProviderType::UNKNOWN;

        /**
         * Identity provider details
         */
        IdentityProviderDetails identityProviderDetails;

        /**
         * Endpoint type
         */
        std::string endpointType;

        /**
         * Logging role
         */
        std::string loggingRole;

        /**
         * Server ID
         */
        std::string serverId;

        /**
         * State
         */
        std::string state;

        /**
         * User count
         */
        long userCount{};

        /**
         * Ports
         */
        std::vector<int> ports;

        /**
         * Concurrency
         */
        long concurrency{};

        /**
         * Protocols
         */
        std::vector<std::string> protocols;

        /**
         * Last started
         */
        system_clock::time_point lastStarted;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

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
                Core::Bson::BsonUtils::SetIntValue(document, "Concurrency", concurrency);
                Core::Bson::BsonUtils::SetDateValue(document, "LastStarted", lastStarted);
                Core::Bson::BsonUtils::SetDateValue(document, "Created", created);
                Core::Bson::BsonUtils::SetDateValue(document, "Modified", modified);

                // Ports
                if (!ports.empty()) {
                    array portsArray;
                    for (const auto &p: ports) {
                        portsArray.append(p);
                    }
                    document.append(kvp("Ports", portsArray));
                }

                // Protocols
                if (!protocols.empty()) {
                    array protocolsArray;
                    for (const auto &p: protocols) {
                        protocolsArray.append(p);
                    }
                    document.append(kvp("Protocols", protocolsArray));
                }

                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend Server tag_invoke(boost::json::value_to_tag<Server>, boost::json::value const &v) {
            Server r;
            r.arn = Core::Json::GetStringValue(v, "Arn");
            r.domain = Core::Json::GetStringValue(v, "Domain");
            r.endpointType = Core::Json::GetStringValue(v, "EndpointType");
            r.loggingRole = Core::Json::GetStringValue(v, "LoggingRole");
            r.serverId = Core::Json::GetStringValue(v, "ServerId");
            r.state = Core::Json::GetStringValue(v, "State");
            r.userCount = Core::Json::GetLongValue(v, "UserCount");
            r.ports = boost::json::value_to<std::vector<int>>(v.at("Ports"));
            r.concurrency = Core::Json::GetLongValue(v, "Concurrency");
            r.protocols = boost::json::value_to<std::vector<std::string>>(v.at("Protocols"));
            r.identityProviderType = IdentityProviderTypeFromString(Core::Json::GetStringValue(v, "IdentityProviderType"));
            r.lastStarted = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "LastStarted"));
            r.created = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "Created"));
            r.modified = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "Modified"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Server const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Arn", obj.arn},
                    {"Domain", obj.domain},
                    {"EndpointType", obj.endpointType},
                    {"LoggingRole", obj.loggingRole},
                    {"ServerId", obj.serverId},
                    {"State", obj.state},
                    {"UserCount", obj.userCount},
                    {"Ports", boost::json::value_from(obj.ports)},
                    {"Concurrency", obj.concurrency},
                    {"Protocols", boost::json::value_from(obj.protocols)},
                    {"IdentityProviderType", IdentityProviderTypeToString(obj.identityProviderType)},
                    {"LastStarted", Core::DateTimeUtils::ToISO8601(obj.lastStarted)},
                    {"Created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"Modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif//AWSMOCK_DTO_TRANSFER_SERVER_H
