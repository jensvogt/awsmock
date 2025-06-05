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
            r.oid = Core::Json::GetStringValue(v, "Id");
            r.name = Core::Json::GetStringValue(v, "Name");
            r.created = Core::DateTimeUtils::FromISO8601(v.at("Created").as_string().data());
            r.modified = Core::DateTimeUtils::FromISO8601(v.at("Modified").as_string().data());
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UserPool const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Id", obj.oid},
                    {"Name", obj.name},
                    {"Created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"LastModified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_MODEL_USER_POOL_H
