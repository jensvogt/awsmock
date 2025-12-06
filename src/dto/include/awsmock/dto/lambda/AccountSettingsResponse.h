//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ACCOUNT_SETTINGS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_ACCOUNT_SETTINGS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/lambda/model/AccountLimit.h>
#include <awsmock/dto/lambda/model/AccountUsage.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Create function response
     *
     * Example:
     * @code{.json}
     * {
     *   "AccountLimit": {
     *     "CodeSizeUnzipped": number,
     *     "CodeSizeZipped": number,
     *     "ConcurrentExecutions": number,
     *     "TotalCodeSize": number,
     *     "UnreservedConcurrentExecutions": number
     *   },
     *   "AccountUsage": {
     *     "FunctionCount": number,
     *     "TotalCodeSize": number
     *   }
     * }
     * @endcode
     */
    struct AccountSettingsResponse final : Common::BaseCounter<AccountSettingsResponse> {

        /**
         * Account limits
         */
        AccountLimit accountLimit;

        /**
         * Account usage
         */
        AccountUsage accountUsage;

      private:

        friend AccountSettingsResponse tag_invoke(boost::json::value_to_tag<AccountSettingsResponse>, boost::json::value const &v) {
            AccountSettingsResponse r;
            r.accountLimit = boost::json::value_to<AccountLimit>(v.at("AccountLimit"));
            r.accountUsage = boost::json::value_to<AccountUsage>(v.at("AccountUsage"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AccountSettingsResponse const &obj) {
            jv = {
                    {"AccountLimit", boost::json::value_from(obj.accountLimit)},
                    {"AccountUsage", boost::json::value_from(obj.accountUsage)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ACCOUNT_SETTINGS_RESPONSE_H
