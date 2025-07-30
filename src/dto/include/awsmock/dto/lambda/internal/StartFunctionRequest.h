//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_START_FUNCTION_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_START_FUNCTION_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Start function request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct StartFunctionRequest final : Common::BaseCounter<StartFunctionRequest> {

        /**
         * Function ARN
         */
        std::string functionArn;

      private:

        friend StartFunctionRequest tag_invoke(boost::json::value_to_tag<StartFunctionRequest>, boost::json::value const &v) {
            StartFunctionRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, StartFunctionRequest const &obj) {
            jv = {
                    {"functionArn", obj.functionArn},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif//AWSMOCK_DTO_LAMBDA_START_FUNCTION_REQUEST_H
