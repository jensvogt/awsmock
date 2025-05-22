//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_H
#define AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Cognito user pool
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserPool final : Common::BaseCounter<UserPool> {

        /**
         * ID
         */
        std::string oid;

        /**
         * Name
         */
        std::string name;

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * User pool ARN
         */
        std::string arn;

        /**
         * User pool domain
         */
        std::string domain;

        /**
         * Number of users
         */
        long userCount{};

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Last modified
         */
        system_clock::time_point modified;

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "id", oid);
                Core::Bson::BsonUtils::SetStringValue(document, "region", region);
                Core::Bson::BsonUtils::SetStringValue(document, "name", name);
                Core::Bson::BsonUtils::SetStringValue(document, "userPoolId", userPoolId);
                Core::Bson::BsonUtils::SetStringValue(document, "arn", arn);
                Core::Bson::BsonUtils::SetDateValue(document, "created", created);
                Core::Bson::BsonUtils::SetDateValue(document, "modified", modified);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend UserPool tag_invoke(boost::json::value_to_tag<UserPool>, boost::json::value const &v) {
            UserPool r;
            r.oid = Core::Json::GetStringValue(v, "oid");
            r.userPoolId = Core::Json::GetStringValue(v, "userPoolId");
            r.arn = Core::Json::GetStringValue(v, "arn");
            r.domain = Core::Json::GetStringValue(v, "domain");
            r.userCount = Core::Json::GetLongValue(v, "userCount");
            r.created = Core::DateTimeUtils::FromISO8601(v.at("Created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("Modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UserPool const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"oid", obj.oid},
                    {"userPoolId", obj.userPoolId},
                    {"arn", obj.arn},
                    {"domain", obj.domain},
                    {"userCount", obj.userCount},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_H
