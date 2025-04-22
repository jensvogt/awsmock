//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ACCOUNT_SETTINGS_RESPONSE_H
#define AWSMOCK_DTO_LAMBDA_ACCOUNT_SETTINGS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseDto.h>
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
    struct AccountSettingsResponse : Common::BaseDto<AccountSettingsResponse> {

        /**
         * Account limits
         */
        AccountLimit accountLimit;

        /**
         * Account usage
         */
        AccountUsage accountUsage;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ACCOUNT_SETTINGS_RESPONSE_H
