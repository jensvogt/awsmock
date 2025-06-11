//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_DETAILS_RESPONSE_H
#define AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_DETAILS_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/RotationRules.h>

namespace AwsMock::Dto::SecretsManager {

    using std::chrono::system_clock;

    /**
     * @brief Get secret details response.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetSecretDetailsResponse final : Common::BaseCounter<GetSecretDetailsResponse> {

        /**
         * secret name
         */
        std::string secretName;

        /**
         * secret URL
         */
        std::string secretUrl;

        /**
         * secret ARN
         */
        std::string secretArn;

        /**
         * secret ID
         */
        std::string secretId;

        /**
         * Secret string
         */
        std::string secretString;

        /**
         * Rotation rules
         */
        RotationRules rotationRules;

        /**
         * Rotation lambda ARN
         */
        std::string rotationLambdaARN;

        /**
         * Last rotation timestamp
         */
        system_clock::time_point lastRotatedDate;

        /**
         * Next rotation timestamp
         */
        system_clock::time_point nextRotatedDate;

        /**
         * Last access timestamp
         */
        system_clock::time_point lastAccessedDate;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

      private:

        friend GetSecretDetailsResponse tag_invoke(boost::json::value_to_tag<GetSecretDetailsResponse>, boost::json::value const &v) {
            GetSecretDetailsResponse r;
            r.secretName = Core::Json::GetStringValue(v, "secretName");
            r.secretUrl = Core::Json::GetStringValue(v, "secretUrl");
            r.secretArn = Core::Json::GetStringValue(v, "secretArn");
            r.secretId = Core::Json::GetStringValue(v, "secretId");
            r.secretString = Core::Json::GetStringValue(v, "secretString");
            r.rotationLambdaARN = Core::Json::GetStringValue(v, "rotationLambdaARN");
            r.rotationRules = boost::json::value_to<RotationRules>(v.at("rotationRules"));
            r.lastRotatedDate = Core::DateTimeUtils::FromISO8601(v.at("lastRotatedDate").as_string().data());
            r.nextRotatedDate = Core::DateTimeUtils::FromISO8601(v.at("nextRotatedDate").as_string().data());
            r.lastAccessedDate = Core::DateTimeUtils::FromISO8601(v.at("lastAccessedDate").as_string().data());
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetSecretDetailsResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"secretName", obj.secretName},
                    {"secretUrl", obj.secretUrl},
                    {"secretArn", obj.secretArn},
                    {"secretId", obj.secretId},
                    {"secretString", obj.secretString},
                    {"rotationLambdaARN", obj.rotationLambdaARN},
                    {"rotationRules", boost::json::value_from(obj.rotationRules)},
                    {"lastRotatedDate", Core::DateTimeUtils::ToISO8601(obj.lastRotatedDate)},
                    {"nextRotatedDate", Core::DateTimeUtils::ToISO8601(obj.nextRotatedDate)},
                    {"lastAccessedDate", Core::DateTimeUtils::ToISO8601(obj.lastAccessedDate)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_GET_SECRET_DETAILS_RESPONSE_H
