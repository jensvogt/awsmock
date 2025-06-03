//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_GET_FUNCTION_COUNTERS_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_GET_FUNCTION_COUNTERS_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    struct GetFunctionCountersRequest final : Common::BaseCounter<GetFunctionCountersRequest> {

        /**
         * function name
         */
        std::string functionArn;

      private:

        friend GetFunctionCountersRequest tag_invoke(boost::json::value_to_tag<GetFunctionCountersRequest>, boost::json::value const &v) {
            GetFunctionCountersRequest r;
            r.functionArn = Core::Json::GetStringValue(v, "functionArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetFunctionCountersRequest const &obj) {
            jv = {
                    {"functionArn", obj.functionArn},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_GET_FUNCTION_COUNTERS_REQUEST_H
