//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_GET_RESULT_COUNTER_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_GET_RESULT_COUNTER_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    struct GetLambdaResultCounterRequest final : Common::BaseCounter<GetLambdaResultCounterRequest> {

        /**
         * Lambda OID
         */
        std::string oid;

      private:

        friend GetLambdaResultCounterRequest tag_invoke(boost::json::value_to_tag<GetLambdaResultCounterRequest>, boost::json::value const &v) {
            GetLambdaResultCounterRequest r;
            r.oid = Core::Json::GetStringValue(v, "oid");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetLambdaResultCounterRequest const &obj) {
            jv = {
                    {"oid", obj.oid},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_GET_RESULT_COUNTER_REQUEST_H
