//
// Created by vogje01 on 06/07/2025
//

#include <awsmock/repository/ApiGatewayDatabase.h>

namespace AwsMock::Database {

    ApiGatewayDatabase::ApiGatewayDatabase() : _databaseName(GetDatabaseName()), _apiGatewayCollectionName("api_gateway"), _memoryDb(ApiGatewayMemoryDb::instance()) {}

}// namespace AwsMock::Database
