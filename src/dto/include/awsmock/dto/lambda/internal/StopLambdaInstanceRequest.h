//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_STOP_LAMBDA_INSTANCE_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_STOP_LAMBDA_INSTANCE_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Stop lambda instance request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct StopLambdaInstanceRequest final : Common::BaseCounter<StopLambdaInstanceRequest> {

        /**
         * Function ARN
         */
        std::string functionArn;

        /**
         * Instance ID
         */
        std::string instanceId;

      private:

        friend StopLambdaInstanceRequest tag_invoke(boost::json::value_to_tag<StopLambdaInstanceRequest>, boost::json::value const &v) {
            StopLambdaInstanceRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            r.instanceId = Core::Json::GetStringValue(v, "instanceId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StopLambdaInstanceRequest const &obj) {
            jv = {
                    {"functionArn", obj.functionArn},
                    {"instanceId", obj.instanceId},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif//AWSMOCK_DTO_LAMBDA_STOP_LAMBDA_INSTANCE_REQUEST_H
