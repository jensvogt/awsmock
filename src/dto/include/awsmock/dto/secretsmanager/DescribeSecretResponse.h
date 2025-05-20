//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETMANAGER_DESCRIBE_SECRET_RESPONSE_H
#define AWSMOCK_DTO_SECRETMANAGER_DESCRIBE_SECRET_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/ReplicationStatus.h>
#include <awsmock/dto/secretsmanager/model/SecretTags.h>
#include <awsmock/dto/secretsmanager/model/VersionIdsToStages.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Describe secret response
     *
     * Example:
     * @code{.json}
     * {
     *   "SecretId": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DescribeSecretResponse final : Common::BaseCounter<DescribeSecretResponse> {

        /**
         * Secret name
         */
        std::string name;

        /**
         * ARN
         */
        std::string arn;

        /**
         * Description
         */
        std::string description;

        /**
         * Last accessed date
         */
        double lastAccessDate;

        /**
         * Last accessed date
         */
        double lastChangedDate;

        /**
         * Last rotation date
         */
        double lastRotatedDate;

        /**
         * Last rotation date
         */
        double nextRotationDate;

        /**
         * Rotation enabled flag
         */
        bool rotationEnabled = false;

        /**
         * Replication status
         */
        ReplicationStatus replicationStatus;

        /**
         * Version IDs to stages
         */
        VersionIdsToStages versionIdsToStages;

        /**
         * Map of Tags
         */
        std::map<std::string, std::string> tags;

      private:

        friend DescribeSecretResponse tag_invoke(boost::json::value_to_tag<DescribeSecretResponse>, boost::json::value const &v) {
            DescribeSecretResponse r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.description = Core::Json::GetStringValue(v, "Description");
            r.lastAccessDate = Core::Json::GetDoubleValue(v, "LastAccessDate");
            r.lastChangedDate = Core::Json::GetDoubleValue(v, "LastChangedDate");
            r.lastRotatedDate = Core::Json::GetDoubleValue(v, "LastRotatedDate");
            r.nextRotationDate = Core::Json::GetDoubleValue(v, "NextRotationDate");
            r.rotationEnabled = Core::Json::GetBoolValue(v, "RotationEnabled");
            r.replicationStatus = boost::json::value_to<ReplicationStatus>(v.at("RotationEnabled"));
            r.versionIdsToStages = boost::json::value_to<VersionIdsToStages>(v.at("VersionIdsToStages"));
            r.tags = boost::json::value_to<std::map<std::string, std::string>>(v.at("Tags"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DescribeSecretResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"ARN", obj.arn},
                    {"Description", obj.description},
                    {"LastAccessDate", obj.lastAccessDate},
                    {"LastChangedDate", obj.lastChangedDate},
                    {"LastRotatedDate", obj.lastRotatedDate},
                    {"NextRotationDate", obj.nextRotationDate},
                    {"RotationEnabled", obj.rotationEnabled},
                    {"ReplicationStatus", boost::json::value_from(obj.replicationStatus)},
                    {"VersionIdsToStages", boost::json::value_from(obj.versionIdsToStages)},
                    {"Tags", boost::json::value_from(obj.tags)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETMANAGER_DESCRIBE_SECRET_RESPONSE_H
