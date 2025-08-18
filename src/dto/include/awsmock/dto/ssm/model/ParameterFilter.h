//
// Created by vogje01 on 8/23/24.
//

#ifndef AWSMOCK_DTO_SSM_MODEL_PARAMETER_FILTERS_H
#define AWSMOCK_DTO_SSM_MODEL_PARAMETER_FILTERS_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/ssm/model/ParameterFilter.h>

namespace AwsMock::Dto::SSM {

    /**
     * @brief SSM parameter filter
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ParameterFilter final : Common::BaseCounter<ParameterFilter> {

        /**
         * The name of the filter.
         */
        std::string key;

        /**
         * For all filters used with DescribeParameters, valid options include Equals and BeginsWith. The Name filter additionally supports
         * the Contains option. (Exception: For filters using the key Path, valid options include Recursive and OneLevel.)
         */
        std::string option;

        /**
         * Filters to limit the request results.
         */
        std::vector<ParameterFilter> parameterFilters;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {
                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Key", key);
                Core::Bson::BsonUtils::SetStringValue(document, "Option", option);

                // Parameter filters
                if (!parameterFilters.empty()) {
                    array jsonParameterFilterArray;
                    for (const auto &element: parameterFilters) {
                        jsonParameterFilterArray.append(element.ToDocument());
                    }
                    document.append(kvp("ParameterFilters", jsonParameterFilterArray));
                }
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

                // Attributes
                key = Core::Bson::BsonUtils::GetStringValue(document, "Key");
                option = Core::Bson::BsonUtils::GetStringValue(document, "Option");

                // Parameter filters
                if (document.view().find("ParameterFilters") != document.view().end()) {
                    for (const bsoncxx::array::view jsonParameterFilterArray = document.view()["ParameterFilters"].get_array().value; const auto &element: jsonParameterFilterArray) {
                        ParameterFilter parameterFilter;
                        parameterFilter.FromDocument(element.get_document().value);
                        parameterFilters.emplace_back(parameterFilter);
                    }
                }

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend ParameterFilter tag_invoke(boost::json::value_to_tag<ParameterFilter>, boost::json::value const &v) {
            ParameterFilter r;
            r.key = Core::Json::GetStringValue(v, "Key");
            r.option = Core::Json::GetStringValue(v, "Option");
            if (Core::Json::AttributeExists(v, "ParameterFilters")) {
                r.parameterFilters = boost::json::value_to<std::vector<ParameterFilter>>(v.at("ParameterFilters"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ParameterFilter const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Key", obj.key},
                    {"Option", obj.option},
                    {"ParameterFilters", boost::json::value_from(obj.parameterFilters)},
            };
        }
    };

}// namespace AwsMock::Dto::SSM

#endif// AWSMOCK_DTO_SSM_MODEL_PARAMETER_FILTERS_H
