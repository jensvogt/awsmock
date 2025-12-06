//
// Created by vogje01 on 5/31/24.
//

#ifndef AWSMOCK_DTO_COGNITO_GROUP_H
#define AWSMOCK_DTO_COGNITO_GROUP_H


// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Cognito user group
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Group final : Common::BaseCounter<Group> {

        /**
         * Group name
         */
        std::string groupName;

        /**
         * User pool ID
         */
        std::string userPoolId;

        /**
         * Description
         */
        std::string description;

        /**
         * Role ARN
         */
        std::string roleArn;

        /**
         * Precedence
         */
        long precedence{};

        /**
         * Created
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modified
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "groupName", groupName);
                Core::Bson::BsonUtils::SetStringValue(document, "userPoolId", userPoolId);
                Core::Bson::BsonUtils::SetStringValue(document, "description", description);
                Core::Bson::BsonUtils::SetLongValue(document, "precedence", precedence);
                Core::Bson::BsonUtils::SetLongValue(document, "created", Core::DateTimeUtils::UnixTimestamp(created));
                Core::Bson::BsonUtils::SetLongValue(document, "modified", Core::DateTimeUtils::UnixTimestamp(modified));
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend Group tag_invoke(boost::json::value_to_tag<Group>, boost::json::value const &v) {
            Group r;
            r.userPoolId = Core::Json::GetStringValue(v, "userPoolId");
            r.groupName = Core::Json::GetStringValue(v, "groupName");
            r.description = Core::Json::GetStringValue(v, "description");
            r.roleArn = Core::Json::GetStringValue(v, "roleArn");
            r.precedence = Core::Json::GetLongValue(v, "precedence");
            r.created = Core::DateTimeUtils::FromISO8601(v.at("Created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("Modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Group const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"userPoolId", boost::json::value_from(obj.userPoolId)},
                    {"groupName", boost::json::value_from(obj.groupName)},
                    {"description", boost::json::value_from(obj.description)},
                    {"roleArn", boost::json::value_from(obj.roleArn)},
                    {"roleArn", boost::json::value_from(obj.roleArn)},
                    {"precedence", boost::json::value_from(obj.precedence)},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_GROUP_H
