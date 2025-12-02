//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/dto/lambda/model/Environment.h>
#include <awsmock/dto/lambda/model/Function.h>
#include <awsmock/entity/lambda/Lambda.h>

namespace AwsMock::Dto::Lambda {

    struct ListFunctionResponse final : Common::BaseCounter<ListFunctionResponse> {

        /**
         * Name of the function
         */
        std::vector<Function> functions;

      private:

        friend ListFunctionResponse tag_invoke(boost::json::value_to_tag<ListFunctionResponse>, boost::json::value const &v) {
            ListFunctionResponse r;
            r.functions = boost::json::value_to<std::vector<Function>>(v.at("Functions"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListFunctionResponse const &obj) {
            jv = {
                    {"Functions", boost::json::value_from(obj.functions)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_LIST_FUNCTION_REQUEST_H
