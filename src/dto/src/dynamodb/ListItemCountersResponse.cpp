//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/ListItemCountersResponse.h>

namespace AwsMock::Dto::DynamoDb {

    std::string ListItemCountersResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetLongValue(document, "total", total);

            if (!itemCounters.empty()) {
                array array;
                for (const auto &itemCounter: itemCounters) {
                    array.append(itemCounter.ToDocument());
                }
                document.append(kvp("itemCounters", array));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
