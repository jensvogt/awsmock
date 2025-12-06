//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRETS_REQUEST_H
#define AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRETS_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/Filter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief List secrets request.
     *
     * Example:
     * @code{.json}
     * {
     *   "Filters": [
     *      {
     *         "Key": "string",
     *         "Values": [ "string" ]
     *      }
     *   ],
     *   "IncludePlannedDeletion": boolean,
     *   "MaxResults": number,
     *   "NextToken": "string",
     *   "SortOrder": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListSecretsRequest final : Common::BaseCounter<ListSecretsRequest> {

        /**
         * Next token
         */
        std::string nextToken;

        /**
         * Sort order
         */
        std::string sortOrder;

        /**
         * Maximal number of results
         */
        long maxResults{};

        /**
         * Include deletion flag
         */
        bool includePlannedDeletion = false;

        /**
         * Selection filters
         */
        std::vector<Filter> filters;

      private:

        friend ListSecretsRequest tag_invoke(boost::json::value_to_tag<ListSecretsRequest>, boost::json::value const &v) {
            ListSecretsRequest r;
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            r.sortOrder = Core::Json::GetStringValue(v, "SortOrder");
            r.maxResults = Core::Json::GetLongValue(v, "MaxResults");
            r.includePlannedDeletion = Core::Json::GetBoolValue(v, "IncludePlannedDeletion");
            if (Core::Json::AttributeExists(v, "Filters"))
                r.filters = boost::json::value_to<std::vector<Filter>>(v.at("Filters"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListSecretsRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"NextToken", obj.nextToken},
                    {"SortOrder", obj.sortOrder},
                    {"MaxResults", obj.maxResults},
                    {"IncludePlannedDeletion", obj.includePlannedDeletion},
                    {"Filters", boost::json::value_from(obj.filters)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRETS_REQUEST_H
