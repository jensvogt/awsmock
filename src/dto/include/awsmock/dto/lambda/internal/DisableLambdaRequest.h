//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_LAMBDA_DISABLE_LAMBDA_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_DISABLE_LAMBDA_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/SortColumn.h>
#include <awsmock/dto/lambda/model/Function.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Disable an application
     *
     * @par
     * Request to start an application.
     *
     * Example:
     * @code{.json}
     * {
     *   "application":
     *   {
     *     "name": "string",
     *     "runtime": "string",
     *     "runType": "string",
     *     "privatePort": number,
     *     "archive": number,
     *     "disabled": "string",
     *     "environment": [{
     *        "key": "string",
     *        "value": "string",
     *        ...
     *     }],
     *     "tags": [{
     *        "key": "string",
     *        "value": "string",
     *        ...
     *     }],
     *     "options": [{
     *        "key": "string",
     *        "value": "string",
     *        ...
     *     }],
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DisableLambdaRequest final : Common::BaseCounter<DisableLambdaRequest> {

        /**
         * Lambda function
         */
        Lambda::Function function;

        /**
         * Application name prefix
         */
        std::string prefix;

        /**
         * Maximal number of results
         */
        long pageSize{};

        /**
         * Page index
         */
        long pageIndex{};

        /**
         * Sort columns
         */
        std::vector<Common::SortColumn> sortColumns;

      private:

        friend DisableLambdaRequest tag_invoke(boost::json::value_to_tag<DisableLambdaRequest>, boost::json::value const &v) {
            DisableLambdaRequest r;
            r.function = boost::json::value_to<Lambda::Function>(v.at("function"));
            r.prefix = Core::Json::GetStringValue(v, "prefix");
            r.pageSize = Core::Json::GetLongValue(v, "pageSize");
            r.pageIndex = Core::Json::GetLongValue(v, "pageIndex");
            if (Core::Json::AttributeExists(v, "sortColumns")) {
                r.sortColumns = boost::json::value_to<std::vector<Common::SortColumn>>(v.at("sortColumns"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DisableLambdaRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"function", boost::json::value_from(obj.function)},
                    {"prefix", obj.prefix},
                    {"pageSize", obj.pageSize},
                    {"pageIndex", obj.pageIndex},
                    {"sortColumns", boost::json::value_from(obj.sortColumns)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_DISABLE_LAMBDA_REQUEST_H
