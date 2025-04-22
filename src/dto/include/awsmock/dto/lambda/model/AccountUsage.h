//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ACCOUNT_USAGE_H
#define AWSMOCK_DTO_LAMBDA_ACCOUNT_USAGE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/lambda/model/EphemeralStorage.h>

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
    struct AccountUsage final : Common::BaseDto<AccountUsage> {

        /**
         * Function count
         */
        int functionCount;

        /**
         * Total code size
         */
        int totalCodeSize;

        /**
         * Convert to a JSON object
         *
         * @return JSON object
         */
        view_or_value<view, value> ToDocument() const;

        /**
         * Convert to a JSON object
         *
         * @return JSON object
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ACCOUNT_USAGE_H
