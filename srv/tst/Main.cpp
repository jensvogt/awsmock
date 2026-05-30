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
#include <awsmock/repository/DynamoDbDatabase.h>
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
        const auto name = AwsMock::Core::Configuration::instance().get<std::string>("awsmock.mongodb.name");
        const auto host = AwsMock::Core::Configuration::instance().get<std::string>("awsmock.mongodb.host");
        const auto user = AwsMock::Core::Configuration::instance().get<std::string>("awsmock.mongodb.user");
        const auto password = AwsMock::Core::Configuration::instance().get<std::string>("awsmock.mongodb.password");
        const int port = AwsMock::Core::Configuration::instance().get<int>("awsmock.mongodb.port");
        const int poolSize = AwsMock::Core::Configuration::instance().get<int>("awsmock.mongodb.pool-size");

        // MongoDB URL
        mongocxx::uri _uri("mongodb://" + user + ":" + password + "@" + host + ":" + std::to_string(port) + "/?maxPoolSize=" + std::to_string(poolSize));

        // Options
        AwsMock::Database::ConnectionPool &_pool = AwsMock::Database::ConnectionPool::instance();
        _pool.Configure();
    }

    /**
     * Initialize the scheduler and start a worker thread so async tasks execute
     */
    void InitializeScheduler() {
        AwsMock::Core::Scheduler::initialize(_ioc);
        _iocWork = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(boost::asio::make_work_guard(_ioc));
        _iocThread = std::thread([this] { _ioc.run(); });
    }

    GlobalTestFixture() {

        // Initialize logging
        AwsMock::Core::LogStream::Initialize();
        AwsMock::Core::LogStream::RemoveConsoleLogs();

        // Create test configuration
        AwsMock::Core::TestUtils::CreateTestConfigurationFile(true);

        // Initialize scheduler
        InitializeScheduler();

        // Initialize database
        InitializeDatabase();
    }

    ~GlobalTestFixture() {
        try {
            const long itemCount = AwsMock::Database::DynamoDbDatabase::instance().DeleteAllItems();
            log_debug << "Items deleted, count: " << itemCount;
            const long tableCount = AwsMock::Database::DynamoDbDatabase::instance().DeleteAllTables();
            log_debug << "Tables deleted, count: " << tableCount;
        } catch (const std::exception &exc) {
            log_error << "Global fixture cleanup failed: " << exc.what();
        }
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
