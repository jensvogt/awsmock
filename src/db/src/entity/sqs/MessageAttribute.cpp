//
// Created by vogje01 on 24/09/2023.
//

#include <awsmock/entity/sqs/MessageAttribute.h>

namespace AwsMock::Database::Entity::SQS {

    view_or_value<view, value> MessageAttribute::ToDocument() const {

        document messageAttributeDoc;
        messageAttributeDoc.append(kvp("stringValue", stringValue));
        messageAttributeDoc.append(kvp("dataType", MessageAttributeTypeToString(dataType)));

        if (!stringListValues.empty()) {
            array stringListValuesDoc{};
            for (const auto &stringListValue: stringListValues) {
                stringListValuesDoc.append(stringListValue);
            }
            messageAttributeDoc.append(kvp("stringListValues", stringListValuesDoc));
        }
        return messageAttributeDoc.extract();
    }

    void MessageAttribute::FromDocument(const view_or_value<view, value> &object) {

        try {

            stringValue = Core::Bson::BsonUtils::GetStringValue(object, "stringValue");
            dataType = MessageAttributeTypeFromString(Core::Bson::BsonUtils::GetStringValue(object, "dataType"));

            // String list values
            if (object.view().find("stringListValues") != object.view().end()) {
                for (const auto &stringListValue: object.view()["stringListValues"].get_array().value) {
                    stringListValues.emplace_back(stringListValue.get_string().value);
                }
            }
        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Database::Entity::SQS
