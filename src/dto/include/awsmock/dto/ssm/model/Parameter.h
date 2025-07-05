//
// Created by vogje01 on 8/23/24.
//

#ifndef AWSMOCK_DTO_SSM_MODEL_PARAMETER_H
#define AWSMOCK_DTO_SSM_MODEL_PARAMETER_H

// C++ standard includes
#include <chrono>
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/ssm/model/ParameterType.h>

namespace AwsMock::Dto::SSM {

    using std::chrono::system_clock;

    /**
     * @brief SSM parameter
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Parameter final : Common::BaseCounter<Parameter> {

        /**
         * Parameter name
         */
        std::string name;

        /**
         * Parameter value
         */
        std::string parameterValue;

        /**
         * Description
         */
        std::string description;

        /**
         * Parameter type
         */
        ParameterType type = ParameterType::string;

        /**
         * KMS key ARN
         */
        std::string kmsKeyArn;

        /**
         * Parameter tier
         */
        std::string tier;

        /**
         * AWS ARN
         */
        std::string arn;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Created timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Name", name);
                Core::Bson::BsonUtils::SetStringValue(document, "Value", parameterValue);
                Core::Bson::BsonUtils::SetStringValue(document, "Type", ParameterTypeToString(type));
                Core::Bson::BsonUtils::SetStringValue(document, "Description", description);
                Core::Bson::BsonUtils::SetStringValue(document, "KmsKeyArn", kmsKeyArn);
                Core::Bson::BsonUtils::SetStringValue(document, "ARN", arn);
                Core::Bson::BsonUtils::SetDateValue(document, "created", created);
                Core::Bson::BsonUtils::SetDateValue(document, "modified", modified);
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

                name = Core::Bson::BsonUtils::GetStringValue(document, "Name");
                parameterValue = Core::Bson::BsonUtils::GetStringValue(document, "Value");
                type = ParameterTypeFromString(Core::Bson::BsonUtils::GetStringValue(document, "Type"));
                description = Core::Bson::BsonUtils::GetStringValue(document, "Description");
                kmsKeyArn = Core::Bson::BsonUtils::GetStringValue(document, "kmsKeyArn");
                arn = Core::Bson::BsonUtils::GetStringValue(document, "ARN");
                created = Core::Bson::BsonUtils::GetDateValue(document, "created");
                modified = Core::Bson::BsonUtils::GetDateValue(document, "created");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend Parameter tag_invoke(boost::json::value_to_tag<Parameter>, boost::json::value const &v) {
            Parameter r;
            r.name = Core::Json::GetStringValue(v, "name");
            r.parameterValue = Core::Json::GetStringValue(v, "value");
            r.description = Core::Json::GetStringValue(v, "description");
            r.type = ParameterTypeFromString(Core::Json::GetStringValue(v, "description"));
            r.kmsKeyArn = Core::Json::GetStringValue(v, "kmsKeyArn");
            r.arn = Core::Json::GetStringValue(v, "arn");
            r.created = Core::Json::GetDatetimeValue(v, "created");
            r.modified = Core::Json::GetDatetimeValue(v, "modified");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Parameter const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"name", obj.name},
                    {"value", obj.parameterValue},
                    {"type", ParameterTypeToString(obj.type)},
                    {"description", obj.description},
                    {"kmsKeyArn", obj.kmsKeyArn},
                    {"arn", obj.arn},
                    {"created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_MODEL_PARAMETER_H
