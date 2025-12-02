//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_COUNTER_H
#define AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_COUNTER_H

// C++ includes
#include <chrono>
#include <string>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Cognito user pool counter
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserPoolCounter final : Common::BaseCounter<UserCounter> {

        /**
         * ID
         */
        std::string id;

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
        long userCount;

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Last modified
         */
        system_clock::time_point modified;

      private:

        friend UserPoolCounter tag_invoke(boost::json::value_to_tag<UserPoolCounter>, boost::json::value const &v) {
            UserPoolCounter r;
            r.id = v.at("id").as_string();
            r.name = v.at("name").as_string();
            r.userPoolId = v.at("userPoolId").as_string();
            r.arn = v.at("arn").as_string();
            r.domain = v.at("domain").as_string();
            r.userCount = v.at("userCount").as_int64();
            r.created = Core::DateTimeUtils::FromISO8601(v.at("created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UserPoolCounter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"id", obj.id},
                    {"name", obj.name},
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

#endif// AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_COUNTER_H
