//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_RESET_FUNCTION_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_RESET_FUNCTION_COUNTERS_REQUEST_H

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Reset function counters
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ResetFunctionCountersRequest final : Common::BaseCounter<ResetFunctionCountersRequest> {

        /**
         * Function name
         */
        std::string functionName;

      private:

        friend ResetFunctionCountersRequest tag_invoke(boost::json::value_to_tag<ResetFunctionCountersRequest>, boost::json::value const &v) {
            ResetFunctionCountersRequest r;
            r.functionName = Core::Json::GetStringValue(v, "functionName");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ResetFunctionCountersRequest const &obj) {
            jv = {
                    {"functionName", obj.functionName},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif//AWSMOCK_DTO_LAMBDA_RESET_FUNCTION_COUNTERS_REQUEST_H
