//
// Created by vogje01 on 5/20/25.
//

#define BOOST_TEST_MODULE DatabaseTests
#include <boost/test/included/unit_test.hpp>

// Awsmock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/utils/ConnectionPool.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

struct GlobalTestFixture {

    /**
     * @brief Initialize the database
     */
    static void InitializeDatabase() {

        // Get database variables
        const auto name = Awsmock::Core::Configuration::instance().get<std::string>("awsmock.mongodb.name");
        const auto host = Awsmock::Core::Configuration::instance().get<std::string>("awsmock.mongodb.host");
        const auto user = Awsmock::Core::Configuration::instance().get<std::string>("awsmock.mongodb.user");
        const auto password = Awsmock::Core::Configuration::instance().get<std::string>("awsmock.mongodb.password");
        const int port = Awsmock::Core::Configuration::instance().get<int>("awsmock.mongodb.port");
        const int poolSize = Awsmock::Core::Configuration::instance().get<int>("awsmock.mongodb.pool-size");

        // MongoDB URL
        mongocxx::uri _uri("mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(port) + "/?maxPoolSize=" + std::to_string(poolSize));

        // Options
        Awsmock::Database::ConnectionPool &_pool = Awsmock::Database::ConnectionPool::instance();
        _pool.Configure();
    }

    GlobalTestFixture() {

        // Initialize logging
        Awsmock::Core::LogStream::Initialize();
        Awsmock::Core::LogStream::RemoveConsoleLogs();

        // Create test configuration
        Awsmock::Core::TestUtils::CreateTestConfigurationFile(true);

        // Initialize database
        InitializeDatabase();
    }

    ~GlobalTestFixture() {
        auto &rf = Awsmock::Database::RepositoryFactory::instance();
        rf.s3Repository()->deleteAllObjects();
        rf.s3Repository()->deleteAllBuckets();
        rf.sqsRepository()->deleteAllMessages();
        rf.sqsRepository()->deleteAllQueues();
        rf.snsRepository()->deleteAllMessages();
        rf.snsRepository()->deleteAllTopics();
        rf.dynamodbRepository()->deleteAllItems();
        rf.dynamodbRepository()->deleteAllTables();
        rf.lambdaRepository()->deleteAllLambdas();
        rf.ssmRepository()->deleteAllParameters();
        rf.apigatewayRepository()->deleteAllKeys();
        rf.apigatewayRepository()->deleteAllUsagePlans();
        rf.applicationRepository()->deleteAllApplications();
        rf.secretsmanagerRepository()->DeleteAllSecrets();
    }
};

BOOST_TEST_GLOBAL_FIXTURE(GlobalTestFixture);
