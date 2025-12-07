//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_MAPPINGS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_MAPPINGS_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/lambda/model/EventSourceMapping.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief List event source mapping response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListEventSourceMappingsResponse final : Common::BaseCounter<ListEventSourceMappingsResponse> {

        /**
         * List of event source mappings.
         */
        std::vector<EventSourceMapping> eventSourceMappings;

      private:

        friend ListEventSourceMappingsResponse tag_invoke(boost::json::value_to_tag<ListEventSourceMappingsResponse>, boost::json::value const &v) {
            ListEventSourceMappingsResponse r;
            r.eventSourceMappings = boost::json::value_to<std::vector<EventSourceMapping>>(v.at("EventSourceMappings"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListEventSourceMappingsResponse const &obj) {
            jv = {
                    {"EventSourceMappings", boost::json::value_from(obj.eventSourceMappings)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_MAPPINGS_RESPONSE_H
