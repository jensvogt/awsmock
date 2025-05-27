//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_ENVIRONMENT_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_LIST_ENVIRONMENT_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/lambda/model/Tags.h>
#include <awsmock/dto/sqs/model/TagCounter.h>

namespace AwsMock::Dto::Lambda {

    struct ListLambdaEnvironmentCountersResponse final : Common::BaseCounter<ListLambdaEnvironmentCountersResponse> {

        /**
         * List of tag counters
         */
        std::vector<std::pair<std::string, std::string>> environmentCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListLambdaEnvironmentCountersResponse tag_invoke(boost::json::value_to_tag<ListLambdaEnvironmentCountersResponse>, boost::json::value const &v) {
            ListLambdaEnvironmentCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.environmentCounters = boost::json::value_to<std::vector<std::pair<std::string, std::string>>>(v.at("environmentCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListLambdaEnvironmentCountersResponse const &obj) {
            jv = {
                    {"total", obj.total},
                    {"sortColumns", boost::json::value_from(obj.environmentCounters)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_ENVIRONMENT_COUNTERS_RESPONSE_H
