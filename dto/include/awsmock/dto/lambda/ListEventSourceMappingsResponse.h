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

        /**
         * @brief Creates a JSON string from the object.
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_EVENT_SOURCE_MAPPINGS_RESPONSE_H
