//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ACCOUNT_LIMIT_H
#define AWSMOCK_DTO_LAMBDA_ACCOUNT_LIMIT_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Create function response
     *
     * Example:
     * @code{.json}
     * "AccountLimit": {
     *   "CodeSizeUnzipped": number,
     *   "CodeSizeZipped": number,
     *   "ConcurrentExecutions": number,
     *   "TotalCodeSize": number,
     *   "UnreservedConcurrentExecutions": number
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz--consulting.com
     */
    struct AccountLimit final : Common::BaseRequest<AccountLimit> {

        /**
         * Code size unzipped limit
         */
        long codeSizeUnzipped{};

        /**
         * Code size zipped limit
         */
        long codeSizeZipped{};

        /**
         * Concurrent executions
         */
        int concurrentExecutions{};

        /**
         * Total code size
         */
        long totalCodeSize{};

        /**
         * Unreserved concurrent executions
         */
        long unreservedConcurrentExecutions{};

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ACCOUNT_LIMIT_H
