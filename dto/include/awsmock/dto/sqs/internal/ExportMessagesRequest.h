//
// Created by vogje01 on 30/05/2023.
//

#pragma once
// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/AwsUtils.h>
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/sqs/model/MessageAttribute.h>

namespace Awsmock::Dto::SQS {

    /**
     * @brief Export messages request
     *
     * @code{.json}
     * {
     *   "queueArn":"string",
     * }
     * @endcode
     */
    struct ExportMessagesRequest final : Common::BaseCounter<ExportMessagesRequest> {

        /**
         * Queue ARN
         */
        std::string queueArn;

      private:

        friend ExportMessagesRequest tag_invoke(boost::json::value_to_tag<ExportMessagesRequest>, boost::json::value const &v) {
            ExportMessagesRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ExportMessagesRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
            };
        }
    };
}// namespace Awsmock::Dto::SQS

