//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SQS {

    /**
     * @brief Get message count response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetMessageCountResponse final : Common::BaseCounter<GetMessageCountResponse> {

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * Message count
         */
        long count{};

    private:
        friend GetMessageCountResponse tag_invoke(boost::json::value_to_tag<GetMessageCountResponse>, boost::json::value const &v) {
            GetMessageCountResponse r;
            r.queueArn = Core::Json::GetBoolValue(v, "queueArn");
            r.count = Core::Json::GetLongValue(v, "count");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetMessageCountResponse const &obj) {
            jv = {
                {"region", obj.region},
                {"user", obj.user},
                {"requestId", obj.requestId},
                {"queueArn", obj.queueArn},
                {"count", obj.count},
            };
        }
    };

} // namespace Awsmock::Dto::SQS
