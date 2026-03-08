//
// Created by vogje01 on 3/8/26.
//

#ifndef AWSMOCK_REPOSITORY_DYNAMODB_UNMARSHALL_H
#define AWSMOCK_REPOSITORY_DYNAMODB_UNMARSHALL_H

// MongoDB includes
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

namespace AwsMock::Database {

    class DynamoDbUnmarshaller {

      public:

        /*static bsoncxx::types::bson_value::value Unnmarshal(const nlohmann::json &dynamoVal) {

            if (dynamoVal.contains("S")) {
                return bsoncxx::types::bson_value::value(dynamoVal["S"].get<std::string>());
            }
            if (dynamoVal.contains("N")) {
                // DynamoDB numbers are strings to avoid precision loss,
                // but MongoDB needs them as actual doubles/longs for comparison
                return bsoncxx::types::bson_value::value(std::stod(dynamoVal["N"].get<std::string>()));
            }
            if (dynamoVal.contains("BOOL")) {
                return bsoncxx::types::bson_value::value(dynamoVal["BOOL"].get<bool>());
            }
            // Add more types (L, M, SS) as needed
            return {nullptr};
        }*/
    };
}// namespace AwsMock::Database

#endif// AWSMOCK_REPOSITORY_DYNAMODB_UNMARSHALL_H
