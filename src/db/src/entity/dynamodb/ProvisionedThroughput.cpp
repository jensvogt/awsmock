//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/entity/dynamodb/ProvisionedThroughput.h>

namespace AwsMock::Database::Entity::DynamoDb {

    view_or_value<view, value> ProvisionedThroughput::ToDocument() const {
        document document;
        Core::Bson::BsonUtils::SetLongValue(document, "ReadCapacityUnits", readCapacityUnits);
        Core::Bson::BsonUtils::SetLongValue(document, "WriteCapacityUnits", writeCapacityUnits);
        Core::Bson::BsonUtils::SetDateValue(document, "LastDecreaseDateTime", lastDecreaseDateTime);
        Core::Bson::BsonUtils::SetDateValue(document, "LastIncreaseDateTime", lastIncreaseDateTime);
        Core::Bson::BsonUtils::SetLongValue(document, "NumberOfDecreasesToday", numberOfDecreasesToday);
        return document.extract();
    }

    void ProvisionedThroughput::FromDocument(const std::optional<view> &document) {
        readCapacityUnits = Core::Bson::BsonUtils::GetIntValue(document, "ReadCapacityUnits");
        writeCapacityUnits = Core::Bson::BsonUtils::GetIntValue(document, "WriteCapacityUnits");
        lastDecreaseDateTime = Core::Bson::BsonUtils::GetDateValue(document, "LastDecreaseDateTime");
        lastIncreaseDateTime = Core::Bson::BsonUtils::GetDateValue(document, "LastIncreaseDateTime");
        numberOfDecreasesToday = Core::Bson::BsonUtils::GetLongValue(document, "NumberOfDecreasesToday");
    }

}// namespace AwsMock::Database::Entity::DynamoDb
