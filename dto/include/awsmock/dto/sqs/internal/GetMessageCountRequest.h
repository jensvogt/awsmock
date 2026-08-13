//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace Awsmock::Dto::SQS {

    /**
     * @brief Get message count request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetMessageCountRequest final : Common::BaseCounter<GetMessageCountRequest> {

        /**
         * Queue ARN
         */
        std::string queueArn;

    private:
        friend GetMessageCountRequest tag_invoke(boost::json::value_to_tag<GetMessageCountRequest>, boost::json::value const &v) {
            GetMessageCountRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetMessageCountRequest const &obj) {
            jv = {
                {"region", obj.region},
                {"user", obj.user},
                {"requestId", obj.requestId},
                {"queueArn", obj.queueArn},
            };
        }
    };
} // namespace Awsmock::Dto::SQS
