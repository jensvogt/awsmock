//
// Created by vogje01 on 21/06/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_ENVIRONMENT_H
#define AWSMOCK_DTO_LAMBDA_ENVIRONMENT_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/lambda/model/Error.h>

namespace AwsMock::Dto::Lambda {

    struct EnvironmentVariables final : Common::BaseCounter<EnvironmentVariables> {

        /**
         * @brief Environment variables
         */
        std::map<std::string, std::string> variables;

        /**
         * Error
         */
        Error error;

        /**
         * @brief Convert to a JSON object
         *
         * @return DTO as BSON document
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {
            try {

                document document;
                if (!variables.empty()) {
                    bsoncxx::builder::basic::document variablesDocument;
                    for (const auto &[fst, snd]: variables) {
                        variablesDocument.append(kvp(fst, snd));
                    }
                    document.append(kvp("Variables", variablesDocument));
                }

                bsoncxx::builder::basic::document errorDocument;
                errorDocument.append(kvp("ErrorCode", error.errorCode));
                errorDocument.append(kvp("Message", error.message));
                document.append(kvp("Error", errorDocument));

                return document.extract();

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Convert to a JSON string
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {
                if (document.view().find("Variables") != document.view().end()) {
                    for (const view jsonObject = document.view()["Variables"].get_document().value; const auto &element: jsonObject) {
                        std::string key = bsoncxx::string::to_string(element.key());
                        const std::string value = bsoncxx::string::to_string(jsonObject[key].get_string().value);
                        variables[key] = value;
                    }
                }
            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend EnvironmentVariables tag_invoke(boost::json::value_to_tag<EnvironmentVariables>, boost::json::value const &v) {
            EnvironmentVariables r;
            r.variables = boost::json::value_to<std::map<std::string, std::string>>(v.at("variables"));
            r.error = boost::json::value_to<Error>(v.at("error"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EnvironmentVariables const &obj) {
            jv = {
                    {"variables", boost::json::value_from(obj.error)},
                    {"error", boost::json::value_from(obj.error)},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif//AWSMOCK_DTO_LAMBDA_ENVIRONMENT_H
