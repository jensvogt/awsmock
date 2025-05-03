//
// Created by vogje01 on 23/09/2023.
//

#include "awsmock/dto/sqs/model/MessageAttribute.h"

namespace AwsMock::Dto::SQS {


    void MessageAttribute::FromDocument(const view_or_value<view, value> &jsonObject) {

        try {
            stringValue = Core::Bson::BsonUtils::GetStringValue(jsonObject, "StringValue");
            dataType = MessageAttributeDataTypeFromString(Core::Bson::BsonUtils::GetStringValue(jsonObject, "DataType"));

            if (jsonObject.view().find("StringListValues") != jsonObject.view().end()) {
                for (const bsoncxx::array::view jsonStringListArray = jsonObject.view()["StringListValues"].get_array().value; const auto &element: jsonStringListArray) {
                    stringListValues.push_back(bsoncxx::string::to_string(element.get_string().value));
                }
            }
        } catch (bsoncxx::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }

    view_or_value<view, value> MessageAttribute::ToDocument() const {

        try {
            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "StringValue", stringValue);
            Core::Bson::BsonUtils::SetStringValue(document, "DataType", MessageAttributeDataTypeToString(dataType));

            // String list values
            if (!stringListValues.empty()) {
                array stringListArrayJson;
                for (const auto &stringListValue: stringListValues) {
                    stringListArrayJson.append(stringListValue);
                }
                document.append(kvp("StringListValues", stringListArrayJson));
            }
            return document.extract();

        } catch (bsoncxx::exception &e) {
            log_error << e.what();
            throw Core::JsonException(e.what());
        }
    }

}// namespace AwsMock::Dto::SQS
