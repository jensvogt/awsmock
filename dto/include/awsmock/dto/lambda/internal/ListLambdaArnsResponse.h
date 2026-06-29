//
// Created by vogje01 on 30/05/2023.
//

#pragma once

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/entity/lambda/Lambda.h>

namespace Awsmock::Dto::Lambda {

    /**
     * @brief List Lambda ARNs response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListLambdaArnsResponse final : Common::BaseCounter<ListLambdaArnsResponse> {

        /**
         * List of lambdas ARNs
         */
        std::vector<std::string> lambdaArns;

      private:

        friend ListLambdaArnsResponse tag_invoke(boost::json::value_to_tag<ListLambdaArnsResponse>, boost::json::value const &v) {
            ListLambdaArnsResponse r;
            r.lambdaArns = boost::json::value_to<std::vector<std::string>>(v.at("lambdaArns"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListLambdaArnsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"lambdaArns", boost::json::value_from(obj.lambdaArns)},
            };
        }
    };
}// namespace Awsmock::Dto::Lambda
