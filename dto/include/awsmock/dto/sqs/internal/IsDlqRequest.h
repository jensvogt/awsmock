//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_IS_DLQ_REQUEST_H
#define AWSMOCK_DTO_SQS_IS_DLQ_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>

namespace Awsmock::Dto::SQS {

    /**
     * @brief Is DLQ request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct IsDlqRequest final : Common::BaseCounter<IsDlqRequest> {

        /**
         * Queue ARN
         */
        std::string queueArn;

      private:

        friend IsDlqRequest tag_invoke(boost::json::value_to_tag<IsDlqRequest>, boost::json::value const &v) {
            IsDlqRequest r;
            r.queueArn = Core::Json::GetStringValue(v, "queueArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, IsDlqRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"queueArn", obj.queueArn},
            };
        }
    };

}// namespace Awsmock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_IS_DLQ_REQUEST_H
