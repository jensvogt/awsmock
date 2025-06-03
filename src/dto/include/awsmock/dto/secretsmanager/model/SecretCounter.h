//
// Created by vogje01 on 4/11/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_SECRET_COUNTER_H
#define AWSMOCK_DTO_SECRETSMANAGER_SECRET_COUNTER_H


// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/RotationRules.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Secrets list filter
     *
     * Example:
     * @code{.json}
     *  {
     *     "Name": "string",
     *     "OwningService": "string",
     *     "ARN": "string",
     *     "Description": "string",
     *     "KmsKeyId": "string",
     *     "PrimaryRegion": "string",
     *     "CreatedDate": number,
     *     "DeletedDate": number,
     *     "LastAccessedDate": number,
     *     "LastChangedDate": number,
     *     "LastRotatedDate": number,
     *     "NextRotationDate": number,
     *     "RotationEnabled": boolean,
     *     "RotationLambdaARN": "string",
     *     "RotationRules": {
     *        "AutomaticallyAfterDays": number,
     *        "Duration": "string",
     *        "ScheduleExpression": "string"
     *     },
     *     "SecretVersionsToStages": {
     *        "string" : [ "string" ]
     *     },
     *     "Tags": [
     *       {
     *          "Key": "string",
     *          "Value": "string"
     *       }
     *    ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct SecretCounter final : Common::BaseCounter<SecretCounter> {

        /**
         * AWS ARN
         */
        std::string secretArn;

        /**
         * Name
         */
        std::string secretName;

        /**
         * Description
         */
        std::string secretId;

        /**
         * Creation date
         */
        long createdDate = 0;

        /**
         * Deleted date
         */
        long deletedDate = 0;

        /**
         * Last access date
         */
        long lastAccessedDate = 0;

        /**
         * Last changed date
         */
        long lastChangedDate = 0;

        /**
         * Last rotation date
         */
        long lastRotatedDate = 0;

        /**
         * Next rotation date
         */
        long nextRotatedDate = 0;

        /**
         * Next rotation date
         */
        bool rotationEnabled = false;

        /**
         * Rotation lambda ARN
         */
        std::string rotationLambdaARN;

        /**
         * Rotation rules
         */
        RotationRules rotationRules;

        /**
         * Creation datetime
         */
        system_clock::time_point created;

        /**
         * Modified datetime
         */
        system_clock::time_point modified;

      private:

        friend SecretCounter tag_invoke(boost::json::value_to_tag<SecretCounter>, boost::json::value const &v) {
            SecretCounter r;
            r.secretName = Core::Json::GetStringValue(v, "secretName");
            r.secretArn = Core::Json::GetStringValue(v, "secretArn");
            r.secretId = Core::Json::GetStringValue(v, "secretId");
            r.createdDate = Core::Json::GetLongValue(v, "createdDate");
            r.deletedDate = Core::Json::GetLongValue(v, "deletedDate");
            r.lastAccessedDate = Core::Json::GetLongValue(v, "lastAccessedDate");
            r.lastChangedDate = Core::Json::GetLongValue(v, "lastChangedDate");
            r.lastRotatedDate = Core::Json::GetLongValue(v, "lastRotatedDate");
            r.nextRotatedDate = Core::Json::GetLongValue(v, "nextRotatedDate");
            r.rotationEnabled = Core::Json::GetLongValue(v, "rotationEnabled");
            r.rotationLambdaARN = Core::Json::GetStringValue(v, "rotationLambdaARN");
            r.rotationRules = boost::json::value_to<RotationRules>(v.at("nextRotatedDate"));
            r.created = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "created"));
            r.modified = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "modified"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, SecretCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"secretName", obj.secretName},
                    {"secretArn", obj.secretArn},
                    {"secretId", obj.secretId},
                    {"createdDate", obj.createdDate},
                    {"deletedDate", obj.deletedDate},
                    {"lastAccessedDate", obj.lastAccessedDate},
                    {"lastChangedDate", obj.lastChangedDate},
                    {"lastRotatedDate", obj.lastRotatedDate},
                    {"nextRotatedDate", obj.nextRotatedDate},
                    {"rotationEnabled", obj.rotationEnabled},
                    {"rotationLambdaARN", obj.rotationLambdaARN},
                    {"rotationRules", boost::json::value_from(obj.rotationRules)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif//A WSMOCK_DTO_SECRETSMANAGER_SECRET_COUNTER_H
