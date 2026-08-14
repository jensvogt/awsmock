//
// Created by vogje01 on 5/20/25.
//

#define BOOST_TEST_MODULE ServiceTests

// C++ standard includes
#include <thread>

// Awsmock includes
#include <awsmock/core/TestUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/scheduler/Scheduler.h>
#include <awsmock/repository/RepositoryFactory.h>
#include <awsmock/utils/ConnectionPool.h>

#include <boost/test/included/unit_test.hpp>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Test"};
}

namespace {
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
            _iocWork = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> >(boost::asio::make_work_guard(_ioc));
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
            const auto &rf = Awsmock::Database::RepositoryFactory::instance();
            std::ignore = rf.s3Repository()->deleteAllObjects();
            std::ignore = rf.s3Repository()->deleteAllBuckets();
            std::ignore = rf.sqsRepository()->deleteAllMessages();
            std::ignore = rf.sqsRepository()->deleteAllQueues();
            std::ignore = rf.snsRepository()->deleteAllMessages();
            std::ignore = rf.snsRepository()->deleteAllTopics();
            std::ignore = rf.dynamodbRepository()->deleteAllItems();
            std::ignore = rf.dynamodbRepository()->deleteAllTables();
            std::ignore = rf.cognitoRepository()->deleteAllGroups({});
            std::ignore = rf.cognitoRepository()->deleteAllUsers();
            std::ignore = rf.cognitoRepository()->deleteAllUserPools();
            std::ignore = rf.kmsRepository()->deleteAllKeys();
            std::ignore = rf.ssmRepository()->deleteAllParameters();
            std::ignore = rf.apigatewayRepository()->deleteAllKeys();
            std::ignore = rf.apigatewayRepository()->deleteAllUsagePlans();
            std::ignore = rf.secretsmanagerRepository()->DeleteAllSecrets();
            _iocWork.reset();
            _ioc.stop();
            if (_iocThread.joinable()) {
                _iocThread.join();
            }
        }

        // Boost IO context and worker thread
        boost::asio::io_context _ioc;
        std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> > _iocWork;
        std::thread _iocThread;
    };
}

BOOST_TEST_GLOBAL_FIXTURE(GlobalTestFixture);
