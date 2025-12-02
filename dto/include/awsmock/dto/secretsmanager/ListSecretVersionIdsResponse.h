//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRETS_VERSION_IDS_RESPONSE_H
#define AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRETS_VERSION_IDS_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/SecretVersion.h>

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
    struct ListSecretVersionIdsResponse final : Common::BaseCounter<ListSecretVersionIdsResponse> {

        /**
         * AWS ARN
         */
        std::string arn;

        /**
         * Secret name
         */
        std::string name;

        /**
         * Next token
         */
        std::string nextToken;

        /**
         * Secret versions
         */
        std::vector<SecretVersion> versions;

      private:

        friend ListSecretVersionIdsResponse tag_invoke(boost::json::value_to_tag<ListSecretVersionIdsResponse>, boost::json::value const &v) {
            ListSecretVersionIdsResponse r;
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.name = Core::Json::GetStringValue(v, "Name");
            r.nextToken = Core::Json::GetStringValue(v, "NextToken");
            r.versions = boost::json::value_to<std::vector<SecretVersion>>(v.at("MaxResults"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListSecretVersionIdsResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ARN", obj.arn},
                    {"Name", obj.name},
                    {"NextToken", obj.nextToken},
                    {"Versions", boost::json::value_from(obj.versions)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_LIST_SECRETS_VERSION_IDS_RESPONSE_H
