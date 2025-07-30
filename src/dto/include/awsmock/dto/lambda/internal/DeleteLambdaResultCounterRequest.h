//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_DELETE_LAMBDA_RESULT_COUNTER_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_DELETE_LAMBDA_RESULT_COUNTER_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    struct DeleteLambdaResultCounterRequest final : Common::BaseCounter<DeleteLambdaResultCounterRequest> {

        /**
         * Lambda OID
         */
        std::string oid;

      private:

        friend DeleteLambdaResultCounterRequest tag_invoke(boost::json::value_to_tag<DeleteLambdaResultCounterRequest>, boost::json::value const &v) {
            DeleteLambdaResultCounterRequest r;
            r.oid = Core::Json::GetStringValue(v, "oid");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteLambdaResultCounterRequest const &obj) {
            jv = {
                    {"oid", obj.oid},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_DELETE_LAMBDA_RESULT_COUNTER_REQUEST_H
