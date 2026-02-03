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
#include <awsmock/core/logging/LogStream.h>
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

                document rootDocument;
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "Name", name);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "Value", parameterValue);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "Type", ParameterTypeToString(type));
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "Description", description);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "KmsKeyArn", kmsKeyArn);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "ARN", arn);
                Core::Bson::BsonUtils::SetStringValue(rootDocument, "Tier", tier);
                Core::Bson::BsonUtils::SetDateValue(rootDocument, "Created", created);
                Core::Bson::BsonUtils::SetDateValue(rootDocument, "Modified", modified);

                if (!tags.empty()) {
                    auto tagsDoc = document{};
                    for (const auto &[fst, snd]: tags) {
                        tagsDoc.append(kvp(fst, snd));
                    }
                    rootDocument.append(kvp("Tags", tagsDoc));
                }
                return rootDocument.extract();

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
                tier = Core::Bson::BsonUtils::GetStringValue(document, "Tier");
                created = Core::Bson::BsonUtils::GetDateValue(document, "Created");
                modified = Core::Bson::BsonUtils::GetDateValue(document, "Modified");

                // Get tags
                if (document.view().find("Tags") != document.view().end()) {
                    tags.clear();
                    for (const view tagsView = document.view()["Tags"].get_document().value; const bsoncxx::document::element &tagElement: tagsView) {
                        std::string key = bsoncxx::string::to_string(tagElement.key());
                        std::string value = bsoncxx::string::to_string(tagsView[key].get_string().value);
                        tags.emplace(key, value);
                    }
                }
            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend Parameter tag_invoke(boost::json::value_to_tag<Parameter>, boost::json::value const &v) {
            Parameter r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.parameterValue = Core::Json::GetStringValue(v, "Value");
            r.description = Core::Json::GetStringValue(v, "Description");
            r.type = ParameterTypeFromString(Core::Json::GetStringValue(v, "Type"));
            r.kmsKeyArn = Core::Json::GetStringValue(v, "KmsKeyArn");
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.tier = Core::Json::GetStringValue(v, "Tier");
            r.created = Core::Json::GetDatetimeValue(v, "Created");
            r.modified = Core::Json::GetDatetimeValue(v, "Modified");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Parameter const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"Value", obj.parameterValue},
                    {"Type", ParameterTypeToString(obj.type)},
                    {"Description", obj.description},
                    {"KmsKeyArn", obj.kmsKeyArn},
                    {"ARN", obj.arn},
                    {"Tier", obj.tier},
                    {"Created", Core::DateTimeUtils::ToISO8601(obj.created)},
                    {"Modified", Core::DateTimeUtils::ToISO8601(obj.modified)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_MODEL_PARAMETER_H
