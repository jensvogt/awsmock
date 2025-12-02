//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_DEAD_LETTER_CONFIG_H
#define AWSMOCK_DTO_LAMBDA_DEAD_LETTER_CONFIG_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    struct DeadLetterConfig final : Common::BaseCounter<DeadLetterConfig> {

        /**
         * Target ARN
         */
        std::string targetArn = {};

        /**
         * Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {
                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "TargetArn", targetArn);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend DeadLetterConfig tag_invoke(boost::json::value_to_tag<DeadLetterConfig>, boost::json::value const &v) {
            DeadLetterConfig r;
            r.targetArn = Core::Json::GetStringValue(v, "targetArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeadLetterConfig const &obj) {
            jv = {
                    {"targetArn", obj.targetArn},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_DEAD_LETTER_CONFIG_H
