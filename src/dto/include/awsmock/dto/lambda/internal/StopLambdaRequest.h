//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_STOP_FUNCTION_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_STOP_FUNCTION_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Stop function request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct StopLambdaRequest final : Common::BaseCounter<StopLambdaRequest> {

        /**
         * Function ARN
         */
        std::string functionArn;

      private:

        friend StopLambdaRequest tag_invoke(boost::json::value_to_tag<StopLambdaRequest>, boost::json::value const &v) {
            StopLambdaRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StopLambdaRequest const &obj) {
            jv = {
                    {"functionArn", obj.functionArn},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif//AWSMOCK_DTO_LAMBDA_STOP_FUNCTION_REQUEST_H
