//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRETS_VERSION_IDS_REQUEST_H
#define AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRETS_VERSION_IDS_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/Filter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief List secrets version IDs request.
     *
     * Example:
     * @code{.json}
     * {
     *   "IncludeDeprecated": boolean,
     *   "MaxResults": number,
     *   "NextToken": "string",
     *   "SecretId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListSecretVersionIdsRequest final : Common::BaseCounter<ListSecretVersionIdsRequest> {

        /**
         * Secret ID
         */
        std::string secretId;

        /**
         * Next token
         */
        std::string nextToken;

        /**
         * Max results
         */
        long maxResults;

        /**
         * Include deletion flag
         */
        bool includeDeprecated = false;

      private:

        friend ListSecretVersionIdsRequest tag_invoke(boost::json::value_to_tag<ListSecretVersionIdsRequest>, boost::json::value const &v) {
            ListSecretVersionIdsRequest r;
            r.secretId = Core::Json::GetStringValue(v, "SecretId");
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            r.maxResults = Core::Json::GetLongValue(v, "MaxResults");
            r.includeDeprecated = Core::Json::GetBoolValue(v, "IncludeDeprecated");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListSecretVersionIdsRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"SecretId", obj.secretId},
                    {"NextToken", obj.nextToken},
                    {"MaxResults", obj.maxResults},
                    {"IncludeDeprecated", obj.includeDeprecated},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRETS_VERSION_IDS_REQUEST_H
