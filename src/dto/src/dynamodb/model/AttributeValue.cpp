//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/model/AttributeValue.h>

namespace AwsMock::Dto::DynamoDb {

    view_or_value<view, value> AttributeValue::ToDocument() const {

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

    void AttributeValue::FromDocument(const view &jsonObject) {

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

    std::string AttributeValue::ToJson() const {
        return Core::Bson::BsonUtils::ToJsonString(ToDocument());
    }

    std::string AttributeValue::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const AttributeValue &r) {
        os << "AttributeValue=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::DynamoDb
