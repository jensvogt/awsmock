//
// Created by vogje01 on 4/11/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_FILTER_H
#define AWSMOCK_DTO_SECRETSMANAGER_FILTER_H


// C++ standard includes
#include <string>
#include <vector>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Secrets list filter
     *
     * Example:
     * @code{.json}
     * "Filters": [
     *   {
     *      "Key": "string",
     *      "Values": [ "string" ]
     *   }
     * ]
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Filter final : Common::BaseCounter<Filter> {

        /**
         * Filter key
         */
        std::string key;

        /**
         * Filter values array
         */
        std::vector<std::string> values;

        /**
         * @brief Converts the DTO to a JSON object.
         *
         * @return DTO as JSON object.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Key", key);

                // Values
                if (!values.empty()) {
                    array valuesArray;
                    for (const auto &e: values) {
                        valuesArray.append(e);
                    }
                    document.append(kvp("Values", valuesArray));
                }
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts the JSON object to DTO.
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                key = Core::Bson::BsonUtils::GetStringValue(document, "Key");

                // Values
                if (document.view().find("Values") != document.view().end()) {
                    for (const bsoncxx::array::view arrayView{document.view()["Values"].get_array().value}; const bsoncxx::array::element &element: arrayView) {
                        values.emplace_back(element.get_string().value);
                    }
                }

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend Filter tag_invoke(boost::json::value_to_tag<Filter>, boost::json::value const &v) {
            Filter r;
            r.key = Core::Json::GetStringValue(v, "Key");
            r.values = boost::json::value_to<std::vector<std::string>>(v.at("Values"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Filter const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Key", obj.key},
                    {"Values", boost::json::value_from(obj.values)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif//AWSMOCK_DTO_SECRETSMANAGER_FILTER_H
