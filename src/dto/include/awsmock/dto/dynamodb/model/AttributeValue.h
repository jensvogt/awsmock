//
// Created by vogje01 on 12/21/23.
//

#ifndef AWSMOCK_DTO_DYNAMODB_ATTRIBUTE_VALUE_H
#define AWSMOCK_DTO_DYNAMODB_ATTRIBUTE_VALUE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * DynamoDB attribute value
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AttributeValue final : Common::BaseCounter<AttributeValue> {

        /**
         * Type
         */
        std::string type;

        /**
         * Value
         */
        std::string attributeValue;

        /**
         * String value
         */
        std::string stringValue;

        /**
         * String set value
         */
        std::vector<std::string> stringSetValue;

        /**
         * Number value
         */
        std::string numberValue;

        /**
         * Number set value
         */
        std::vector<std::string> numberSetValue;

        /**
         * Boolean value
         */
        std::shared_ptr<bool> boolValue;

        /**
         * Null value
         */
        std::shared_ptr<bool> nullValue;

        /**
         * @brief Convert from JSON object.
         *
         * @param jsonObject JSON object
         */
        void FromDocument(const view &jsonObject) {

            try {
                for (const bsoncxx::document::element &ele: jsonObject) {
                    if (ele.key() == "S") {
                        stringValue = bsoncxx::string::to_string(jsonObject["S"].get_string().value);
                    } else if (ele.key() == "SS") {
                        for (bsoncxx::array::view jsonArray = jsonObject["SS"].get_array().value; const auto &value: jsonArray) {
                            stringSetValue.emplace_back(value.get_string().value);
                        }
                    } else if (ele.key() == "N") {
                        numberValue = bsoncxx::string::to_string(jsonObject["N"].get_string().value);
                    } else if (ele.key() == "NS") {
                        for (bsoncxx::array::view jsonArray = jsonObject["NS"].get_array().value; const auto &value: jsonArray) {
                            numberSetValue.emplace_back(value.get_string().value);
                        }
                    } else if (ele.key() == "BOOL") {
                        boolValue = std::make_shared<bool>(jsonObject["BOOL"].get_bool().value);
                    } else if (ele.key() == "nullptr") {
                        nullValue = std::make_shared<bool>(true);
                    }
                }
            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Convert to JSON value
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "S", stringValue);
                Core::Bson::BsonUtils::SetStringValue(document, "N", numberValue);

                if (!stringSetValue.empty()) {
                    array jsonArray;
                    for (const auto &value: stringSetValue) {
                        jsonArray.append(value);
                    }
                    document.append(kvp("SS", jsonArray));
                }

                if (!numberSetValue.empty()) {
                    array jsonArray;
                    for (const auto &value: numberSetValue) {
                        jsonArray.append(value);
                    }
                    document.append(kvp("NS", jsonArray));
                }

                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend AttributeValue tag_invoke(boost::json::value_to_tag<AttributeValue>, boost::json::value const &v) {
            AttributeValue r;
            r.type = Core::Json::GetStringValue(v, "type");
            r.stringValue = Core::Json::GetStringValue(v, "stringValue");
            r.numberValue = Core::Json::GetStringValue(v, "numberValue");
            r.boolValue = std::make_shared<bool>(Core::Json::GetBoolValue(v, "boolValue"));
            r.nullValue = std::make_shared<bool>(Core::Json::GetBoolValue(v, "nullValue"));
            if (Core::Json::AttributeExists(v, "stringSetValue")) {
                r.stringSetValue = boost::json::value_to<std::vector<std::string>>(v.at("stringSetValue"));
            }
            if (Core::Json::AttributeExists(v, "numberSetValue")) {
                r.numberSetValue = boost::json::value_to<std::vector<std::string>>(v.at("numberSetValue"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AttributeValue const &obj) {
            jv = {
                    {"type", obj.type},
                    {"stringValue", obj.stringValue},
                    {"numberValue", obj.numberValue},
                    {"stringSetValue", boost::json::value_from(obj.stringSetValue)},
                    {"numberSetValue", boost::json::value_from(obj.numberSetValue)},
            };
            if (obj.boolValue) {
                jv.at("boolValue") = *obj.boolValue;
            }
            if (obj.nullValue) {
                jv.at("nullValue") = *obj.nullValue;
            }
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_ATTRIBUTE_VALUE_H
