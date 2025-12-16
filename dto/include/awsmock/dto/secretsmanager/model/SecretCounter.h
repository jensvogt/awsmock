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
         * Secret string
         */
        std::string secretString;

        /**
         * Creation date
         */
        system_clock::time_point createdDate;

        /**
         * Deleted date
         */
        system_clock::time_point deletedDate;

        /**
         * Last access date
         */
        system_clock::time_point lastAccessedDate;

        /**
         * Last changed date
         */
        system_clock::time_point lastChangedDate;

        /**
         * Last rotation date
         */
        system_clock::time_point lastRotatedDate;

        /**
         * Next rotation date
         */
        system_clock::time_point nextRotatedDate;

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
            r.secretString = Core::Json::GetStringValue(v, "secretString");
            r.createdDate = Core::Json::GetDatetimeValue(v, "createdDate");
            r.deletedDate = Core::Json::GetDatetimeValue(v, "deletedDate");
            r.lastAccessedDate = Core::Json::GetDatetimeValue(v, "lastAccessedDate");
            r.lastChangedDate = Core::Json::GetDatetimeValue(v, "lastChangedDate");
            r.lastRotatedDate = Core::Json::GetDatetimeValue(v, "lastRotatedDate");
            r.nextRotatedDate = Core::Json::GetDatetimeValue(v, "nextRotatedDate");
            r.rotationEnabled = Core::Json::GetBoolValue(v, "rotationEnabled");
            r.rotationLambdaARN = Core::Json::GetStringValue(v, "rotationLambdaARN");
            r.created = Core::Json::GetDatetimeValue(v, "created");
            r.modified = Core::Json::GetDatetimeValue(v, "modified");
            if (Core::Json::AttributeExists(v, "rotationRules")) {
                r.rotationRules = boost::json::value_to<RotationRules>(v.at("rotationRules"));
            }
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
                    {"secretString", obj.secretString},
                    {"createdDate", Core::DateTimeUtils::ToISO8601(obj.createdDate)},
                    {"deletedDate", Core::DateTimeUtils::ToISO8601(obj.deletedDate)},
                    {"lastAccessedDate", Core::DateTimeUtils::ToISO8601(obj.lastAccessedDate)},
                    {"lastChangedDate", Core::DateTimeUtils::ToISO8601(obj.lastChangedDate)},
                    {"lastRotatedDate", Core::DateTimeUtils::ToISO8601(obj.lastRotatedDate)},
                    {"nextRotatedDate", Core::DateTimeUtils::ToISO8601(obj.nextRotatedDate)},
                    {"rotationEnabled", obj.rotationEnabled},
                    {"rotationLambdaARN", obj.rotationLambdaARN},
                    {"rotationRules", boost::json::value_from(obj.rotationRules)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_SECRET_COUNTER_H
