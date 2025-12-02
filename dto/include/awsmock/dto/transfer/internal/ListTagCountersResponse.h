//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_LIST_TAG_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_TRANSFER_LIST_TAG_COUNTERS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Transfer {

    struct ListTagCountersResponse final : Common::BaseCounter<ListTagCountersResponse> {

        /**
         * List of tag counters
         */
        std::map<std::string, std::string> tagCounters;

        /**
         * Total number of tags
         */
        long total = 0;

      private:

        friend ListTagCountersResponse tag_invoke(boost::json::value_to_tag<ListTagCountersResponse>, boost::json::value const &v) {
            ListTagCountersResponse r;
            r.total = Core::Json::GetLongValue(v, "total");
            if (Core::Json::AttributeExists(v, "tagCounters")) {
                r.tagCounters = boost::json::value_to<std::map<std::string, std::string>>(v.at("tagCounters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListTagCountersResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"total", obj.total}};

            // Convert from map to key/value vector
            if (!obj.tagCounters.empty()) {
                boost::json::array tagArray;
                for (const auto &[fst, snd]: obj.tagCounters) {
                    tagArray.push_back(boost::json::object{{"key", fst}, {"value", snd}});
                }
                jv.as_object()["tagCounters"] = tagArray;
            }
        }
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_LIST_TAG_COUNTERS_RESPONSE_H
