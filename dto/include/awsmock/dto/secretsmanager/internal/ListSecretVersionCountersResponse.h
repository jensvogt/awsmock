//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRET_VERSION_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRET_VERSION_COUNTERS_RESPONSE_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/SecretCounter.h>
#include <awsmock/dto/secretsmanager/model/SecretVersionCounter.h>

namespace AwsMock::Dto::SecretsManager {

    struct ListSecretVersionCountersResponse final : Common::BaseCounter<ListSecretVersionCountersResponse> {

        /**
         * List of secret counters
         */
        std::vector<SecretVersionCounter> secretVersionCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListSecretVersionCountersResponse tag_invoke(boost::json::value_to_tag<ListSecretVersionCountersResponse>, boost::json::value const &v) {
            ListSecretVersionCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "secretVersionCounters")) {
                r.secretVersionCounters = boost::json::value_to<std::vector<SecretVersionCounter>>(v.at("secretVersionCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListSecretVersionCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total},
                    {"secretVersionCounters", boost::json::value_from(obj.secretVersionCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRET_VERSION_COUNTERS_RESPONSE_H
