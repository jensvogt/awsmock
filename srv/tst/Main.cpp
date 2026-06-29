//
// Created by vogje01 on 5/20/25.
//

#define BOOST_TEST_MODULE ServiceTests
#include <boost/test/included/unit_test.hpp>

// C++ standard includes
#include <thread>

// Awsmock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/repository/dynamodb/DynamoDbMongoRepository.h>
#include <awsmock/utils/ConnectionPool.h>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

struct GlobalTestFixture {

    /**
     * Initialize the database
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

    /**
     * Initialize the scheduler and start a worker thread so async tasks execute
     */
    void InitializeScheduler() {
        Awsmock::Core::Scheduler::initialize(_ioc);
        _iocWork = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(boost::asio::make_work_guard(_ioc));
        _iocThread = std::thread([this] { _ioc.run(); });
    }

    GlobalTestFixture() {

        // Initialize logging
        Awsmock::Core::LogStream::Initialize();
        Awsmock::Core::LogStream::RemoveConsoleLogs();

        // Create test configuration
        Awsmock::Core::TestUtils::CreateTestConfigurationFile(true);

        // Initialize scheduler
        InitializeScheduler();

        // Initialize database
        InitializeDatabase();
    }

    ~GlobalTestFixture() {
        _iocWork.reset();
        _ioc.stop();
        if (_iocThread.joinable()) {
            _iocThread.join();
        }
    }

    // Boost IO context and worker thread
    boost::asio::io_context _ioc;
    std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> _iocWork;
    std::thread _iocThread;
};

BOOST_TEST_GLOBAL_FIXTURE(GlobalTestFixture);
