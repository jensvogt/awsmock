//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_START_LAMBDA_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_START_LAMBDA_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Start function request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct StartLambdaRequest final : Common::BaseCounter<StartLambdaRequest> {

        /**
         * Function ARN
         */
        std::string functionArn;

      private:

        friend StartLambdaRequest tag_invoke(boost::json::value_to_tag<StartLambdaRequest>, boost::json::value const &v) {
            StartLambdaRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StartLambdaRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"functionArn", obj.functionArn},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif//AWSMOCK_DTO_LAMBDA_START_LAMBDA_REQUEST_H
