//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Delete function request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteFunctionRequest final : Common::BaseCounter<DeleteFunctionRequest> {

        /**
         * Name of the function
         */
        std::string functionName;

        /**
         * Qualifier
         */
        std::string qualifier;

      private:

        friend DeleteFunctionRequest tag_invoke(boost::json::value_to_tag<DeleteFunctionRequest>, boost::json::value const &v) {
            DeleteFunctionRequest r;
            r.functionName = Core::Json::GetStringValue(v, "FunctionName");
            r.qualifier = Core::Json::GetStringValue(v, "Qualifier");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteFunctionRequest const &obj) {
            jv = {
                    {"FunctionName", obj.functionName},
                    {"Qualifier", obj.qualifier},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_REQUEST_H
