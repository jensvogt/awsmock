//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRET_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRET_COUNTERS_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/SecretCounter.h>
#include <awsmock/dto/sqs/model/QueueCounter.h>

namespace AwsMock::Dto::SecretsManager {

    struct ListSecretCountersResponse final : Common::BaseCounter<ListSecretCountersResponse> {

        /**
         * List of secret counters
         */
        std::vector<SecretCounter> secretCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListSecretCountersResponse tag_invoke(boost::json::value_to_tag<ListSecretCountersResponse>, boost::json::value const &v) {
            ListSecretCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "c")) {
                r.secretCounters = boost::json::value_to<std::vector<SecretCounter>>(v.at("secretCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListSecretCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"secretCounters", boost::json::value_from(obj.secretCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRET_COUNTERS_RESPONSE_H
