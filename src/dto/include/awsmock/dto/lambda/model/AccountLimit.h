//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ACCOUNT_LIMIT_H
#define AWSMOCK_DTO_LAMBDA_ACCOUNT_LIMIT_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

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
    struct AccountLimit final : Common::BaseCounter<AccountLimit> {

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
        long concurrentExecutions{};

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

      private:

        friend AccountLimit tag_invoke(boost::json::value_to_tag<AccountLimit>, boost::json::value const &v) {
            AccountLimit r;
            r.codeSizeUnzipped = Core::Json::GetLongValue(v, "CodeSizeUnzipped");
            r.codeSizeZipped = Core::Json::GetLongValue(v, "CodeSizeZipped");
            r.concurrentExecutions = Core::Json::GetLongValue(v, "ConcurrentExecutions");
            r.totalCodeSize = Core::Json::GetLongValue(v, "TotalCodeSize");
            r.unreservedConcurrentExecutions = Core::Json::GetLongValue(v, "UnreservedConcurrentExecutions");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AccountLimit const &obj) {
            jv = {
                    {"CodeSizeUnzipped", obj.codeSizeUnzipped},
                    {"CodeSizeZipped", obj.codeSizeZipped},
                    {"ConcurrentExecutions", obj.concurrentExecutions},
                    {"TotalCodeSize", obj.totalCodeSize},
                    {"UnreservedConcurrentExecutions", obj.unreservedConcurrentExecutions},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_ACCOUNT_LIMIT_H
