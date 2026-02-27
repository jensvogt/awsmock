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
#include <awsmock/dto/secretsmanager/model/Tag.h>
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
         * KMS key ID
         */
        std::string kmsKeyId;

        /**
         * Rotation lambda ARN
         */
        std::string rotationLambdaARN;

        /**
         * Create date
         */
        long createdDate{};

        /**
         * Deleted date
         */
        long deletedDate{};

        /**
         * Last accessed date
         */
        long lastAccessDate{};

        /**
         * Last accessed date
         */
        long lastChangedDate{};

        /**
         * Last rotation date
         */
        long lastRotatedDate{};

        /**
         * Last rotation date
         */
        long nextRotationDate{};

        /**
         * Owning service
         */
        std::string owningService{};

        /**
         * Rotation enabled flag
         */
        bool rotationEnabled = false;

        /**
         * Replication status
         */
        std::vector<ReplicationStatus> replicationStatus;

        /**
         * Version IDs to stages
         */
        VersionIdsToStages versionIdsToStages;

        /**
         * Map of Tags
         */
        std::vector<Tag> tags;

      private:

        friend DescribeSecretResponse tag_invoke(boost::json::value_to_tag<DescribeSecretResponse>, boost::json::value const &v) {
            DescribeSecretResponse r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.description = Core::Json::GetStringValue(v, "Description");
            r.owningService = Core::Json::GetStringValue(v, "OwningService");
            r.rotationLambdaARN = Core::Json::GetStringValue(v, "RotationLambdaARN");
            r.createdDate = Core::Json::GetLongValue(v, "CreatedDate");
            r.deletedDate = Core::Json::GetLongValue(v, "DeletedDate");
            r.lastAccessDate = Core::Json::GetLongValue(v, "LastAccessDate");
            r.lastChangedDate = Core::Json::GetLongValue(v, "LastChangedDate");
            r.lastRotatedDate = Core::Json::GetLongValue(v, "LastRotatedDate");
            r.nextRotationDate = Core::Json::GetLongValue(v, "NextRotationDate");
            r.rotationEnabled = Core::Json::GetBoolValue(v, "RotationEnabled");
            if (Core::Json::AttributeExists(v, "ReplicationStatus")) {
                r.replicationStatus = boost::json::value_to<std::vector<ReplicationStatus>>(v.at("ReplicationStatus"));
            }
            if (Core::Json::AttributeExists(v, "VersionIdsToStages")) {
                r.versionIdsToStages = boost::json::value_to<VersionIdsToStages>(v.at("VersionIdsToStages"));
            }
            if (Core::Json::AttributeExists(v, "Tags")) {
                r.tags = boost::json::value_to<std::vector<Tag>>(v.at("Tags"));
            }
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
                    {"OwningService", obj.owningService},
                    {"RotationLambdaARN", obj.rotationLambdaARN},
                    {"CreatedDate", obj.createdDate},
                    {"DeletedDate", obj.deletedDate},
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
