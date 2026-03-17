//
// Created by vogje01 on 01/09/2022.
//

// C++ includes
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE DbTests

// C++ includes
#include <memory>

// Boost includes
#include <boost/thread.hpp>

// AwsMock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/utils/ConnectionPool.h>

AwsMock::Database::ConnectionPool &pool = AwsMock::Database::ConnectionPool::instance();

#include "CognitoDatabaseTests.cpp"
#include "CognitoMemoryDbTests.cpp"
#include "LambdaDatabaseTests.cpp"
#include "LambdaMemoryDbTests.cpp"
#include "ModuleDatabaseTests.cpp"
#include "ModuleMemoryDbTests.cpp"
#include "S3DatabaseTests.cpp"
#include "S3MemoryDbTests.cpp"
#include "SNSDatabaseTests.cpp"
#include "SNSMemoryDbTests.cpp"
#include "SQSDatabaseTests.cpp"
#include "SQSMemoryDbTests.cpp"
#include "SSMDatabaseTests.cpp"
#include "SSMMemoryDbTests.cpp"
#include "SecretsManagerDatabaseTests.cpp"
#include "SecretsManagerMemoryDbTests.cpp"
#include "TransferDatabaseTests.cpp"
#include "TransferMemoryDbTests.cpp"

/**
 * Initialize database
 */
static void InitializeDatabase() {

    // Get database variables
    const auto name = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.name");
    const auto host = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.host");
    const auto user = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.user");
    const auto password = AwsMock::Core::Configuration::instance().GetValue<std::string>("awsmock.mongodb.password");
    const int _port = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.mongodb.port");
    const int poolSize = AwsMock::Core::Configuration::instance().GetValue<int>("awsmock.mongodb.pool-size");

    // MongoDB URL
    mongocxx::uri _uri("mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(_port) + "/?maxPoolSize=" + std::to_string(poolSize));

    // Options
    pool.Configure();
}

bool init_unit_test() {

    AwsMock::Core::LogStream::Initialize();
    AwsMock::Core::LogStream::RemoveConsoleLogs();
    AwsMock::Core::TestUtils::CreateTestConfigurationFile();

    InitializeDatabase();

    return true;
}

// entry point:
int main(const int argc, char *argv[]) {

    return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
