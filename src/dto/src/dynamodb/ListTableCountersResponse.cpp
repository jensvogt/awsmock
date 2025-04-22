//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/ListTableCountersResponse.h>

namespace AwsMock::Dto::DynamoDb {

    std::string ListTableCountersResponse::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetLongValue(document, "total", total);

            if (!tableCounters.empty()) {
                array array;
                for (const auto &tableCounter: tableCounters) {
                    array.append(tableCounter.ToDocument());
                }
                document.append(kvp("tableCounters", array));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::DynamoDb
