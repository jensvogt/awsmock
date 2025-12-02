//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_TAG_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_LIST_TAG_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/sqs/model/TagCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief List lambda tag counter response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListLambdaTagCountersResponse final : Common::BaseCounter<ListLambdaTagCountersResponse> {

        /**
         * List of tag counters
         */
        std::vector<std::pair<std::string, std::string>> tagCounters;

        /**
         * Total number of queues
         */
        long total = 0;

      private:

        friend ListLambdaTagCountersResponse tag_invoke(boost::json::value_to_tag<ListLambdaTagCountersResponse>, boost::json::value const &v) {
            ListLambdaTagCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            r.tagCounters = boost::json::value_to<std::vector<std::pair<std::string, std::string>>>(v.at("tagCounters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListLambdaTagCountersResponse const &obj) {
            boost::json::array tagArray;
            for (const auto &[fst, snd]: obj.tagCounters) {
                boost::json::object tagObject;
                tagObject["key"] = fst;
                tagObject["value"] = snd;
                tagArray.push_back(tagObject);
            }
            jv = {
                    {"total", obj.total},
                    {"tagCounters", tagArray},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_TAG_COUNTERS_RESPONSE_H
