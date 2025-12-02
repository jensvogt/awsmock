//
// Created by vogje01 on 4/8/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_ROTATION_RULES_H
#define AWSMOCK_DTO_SECRETSMANAGER_ROTATION_RULES_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Rotation rules
     *
     * Example:
     * @code{.json}
     * {
     *   "AutomaticallyAfterDays": number,
     *   "Duration": "string",
     *   "ScheduleExpression": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RotationRules final : Common::BaseCounter<RotationRules> {

        /**
         * Automatic rotation period
         */
        long automaticallyAfterDays = 0;

        /**
         * Duration
         */
        std::string duration;

        /**
         * Duration
         */
        std::string scheduleExpression;

        /**
         * @brief Converts the DTO to a JSON object.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetLongValue(document, "AutomaticallyAfterDays", automaticallyAfterDays);
                Core::Bson::BsonUtils::SetStringValue(document, "Duration", duration);
                Core::Bson::BsonUtils::SetStringValue(document, "ScheduleExpression", scheduleExpression);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param document JSON object.
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                automaticallyAfterDays = Core::Bson::BsonUtils::GetLongValue(document, "AutomaticallyAfterDays");
                duration = Core::Bson::BsonUtils::GetStringValue(document, "Duration");
                scheduleExpression = Core::Bson::BsonUtils::GetStringValue(document, "ScheduleExpression");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend RotationRules tag_invoke(boost::json::value_to_tag<RotationRules>, boost::json::value const &v) {
            RotationRules r;
            r.automaticallyAfterDays = Core::Json::GetLongValue(v, "AutomaticallyAfterDays");
            r.duration = Core::Json::GetStringValue(v, "Duration");
            r.scheduleExpression = Core::Json::GetStringValue(v, "ScheduleExpression");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, RotationRules const &obj) {
            jv = {
                    {"AutomaticallyAfterDays", obj.automaticallyAfterDays},
                    {"Duration", obj.duration},
                    {"ScheduleExpression", obj.scheduleExpression},
            };
        }
    };

}//namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_ROTATION_RULES_H
