//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_LIST_PROTOCOL_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_TRANSFER_LIST_PROTOCOL_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/transfer/model/Protocol.h>

namespace AwsMock::Dto::Transfer {

    struct ProtocolCounter final : Common::BaseCounter<ProtocolCounter> {

        /**
         * Protocol
         */
        ProtocolType protocol = ProtocolType::UNKNOWN;

        /**
         * Port
         */
        long port = 0;

        /**
         * @brief Crete JSON document from object
         *
         * @return JSON document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                auto protocolDoc = document{};
                Core::Bson::BsonUtils::SetStringValue(protocolDoc, "Protocol", ProtocolTypeToString(protocol));
                Core::Bson::BsonUtils::SetLongValue(protocolDoc, "Port", port);
                return protocolDoc.extract();

            } catch (const std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend ProtocolCounter tag_invoke(boost::json::value_to_tag<ProtocolCounter>, boost::json::value const &v) {
            ProtocolCounter r;
            r.port = Core::Json::GetLongValue(v, "port");
            r.protocol = ProtocolTypeFromString(Core::Json::GetStringValue(v, "protocol"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ProtocolCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"port", obj.port},
                    {"protocol", ProtocolTypeToString(obj.protocol)},
            };
        }
    };

    struct ListProtocolCountersResponse final : Common::BaseCounter<ListProtocolCountersResponse> {

        /**
         * List of attribute counters
         */
        std::vector<ProtocolCounter> protocolCounters;

        /**
         * Total number of users
         */
        long total = 0;

      private:

        friend ListProtocolCountersResponse tag_invoke(boost::json::value_to_tag<ListProtocolCountersResponse>, boost::json::value const &v) {
            ListProtocolCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.protocolCounters = boost::json::value_to<std::vector<ProtocolCounter>>(v.at("protocolCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListProtocolCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"protocolCounters", boost::json::value_from(obj.protocolCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_LIST_PROTOCOL_COUNTERS_RESPONSE_H
