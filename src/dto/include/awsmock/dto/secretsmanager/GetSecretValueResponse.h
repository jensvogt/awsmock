//
// Created by vogje01 on 4/9/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_VALUE_RESPONSE_H
#define AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_VALUE_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * Get a secret value
     *
     * Example:
     * @code{.json}
     * {
     *   "Name": "string",
     *   "ARN": "string",
     *   "CreatedDate": number,
     *   "SecretBinary": blob,
     *   "SecretString": "string",
     *   "VersionId": "string",
     *   "VersionStages": [ "string" ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetSecretValueResponse final : Common::BaseCounter<GetSecretValueResponse> {

        /*+
         * Name
         */
        std::string name;

        /**
         * ARN
         */
        std::string arn;

        /**
         * Created date
         */
        long createdDate = -1;

        /**
         * Secret string
         */
        std::string secretString;

        /**
         * Base64 encoded secret binary data
         */
        std::string secretBinary;

        /**
         * Version ID
         */
        std::string versionId;

        /**
         * Version ID
         */
        std::vector<std::string> versionStages;

      private:

        friend GetSecretValueResponse tag_invoke(boost::json::value_to_tag<GetSecretValueResponse>, boost::json::value const &v) {
            GetSecretValueResponse r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.createdDate = Core::Json::GetLongValue(v, "CreatedDate");
            r.secretString = Core::Json::GetStringValue(v, "SecretString");
            r.secretBinary = Core::Json::GetStringValue(v, "SecretBinary");
            r.versionId = Core::Json::GetStringValue(v, "VersionId");
            r.versionStages = boost::json::value_to<std::vector<std::string>>(v.at("VersionId"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetSecretValueResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"ARN", obj.arn},
                    {"CreatedDate", obj.createdDate},
                    {"SecretString", obj.secretString},
                    {"SecretBinary", obj.secretBinary},
                    {"VersionId", obj.versionId},
                    {"VersionStages", boost::json::value_from(obj.versionStages)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_VALUE_RESPONSE_H
