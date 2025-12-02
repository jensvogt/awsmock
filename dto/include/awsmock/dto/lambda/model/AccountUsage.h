//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ACCOUNT_USAGE_H
#define AWSMOCK_DTO_LAMBDA_ACCOUNT_USAGE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Account usage model
     *
     * Example:
     * @code{.json}
     * "AccountUsage": {
     *   "FunctionCount": number,
     *   "TotalCodeSize": number
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz--consulting.com
     */
    struct AccountUsage final : Common::BaseCounter<AccountUsage> {

        /**
         * Function count
         */
        long functionCount{};

        /**
         * Total code size
         */
        long totalCodeSize{};

        /**
         * Convert to a JSON object
         *
         * @return JSON object
         */
        view_or_value<view, value> ToDocument() const;

      private:

        friend AccountUsage tag_invoke(boost::json::value_to_tag<AccountUsage>, boost::json::value const &v) {
            AccountUsage r;
            r.functionCount = Core::Json::GetLongValue(v, "FunctionCount");
            r.totalCodeSize = Core::Json::GetLongValue(v, "TotalCodeSize");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AccountUsage const &obj) {
            jv = {
                    {"FunctionCount", obj.functionCount},
                    {"TotalCodeSize", obj.totalCodeSize},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ACCOUNT_USAGE_H
