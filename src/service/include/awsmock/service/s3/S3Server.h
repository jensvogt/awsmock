//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_S3_SERVER_H
#define AWSMOCK_SERVICE_S3_SERVER_H

// Boost includes
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/asio/thread_pool.hpp>
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

// AwsMock includes
#include <awsmock/core/LogStream.h>
#include <awsmock/core/scheduler/PeriodicScheduler.h>
#include <awsmock/core/scheduler/PeriodicTask.h>
#include <awsmock/service/common/AbstractServer.h>
#include <awsmock/service/s3/S3Service.h>

namespace AwsMock::Service {

    using namespace boost::filesystem;

    /**
     * @brief S3 module server
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class S3Server final : public AbstractServer {

      public:

        /**
         * @brief Constructor
         */
        explicit S3Server(Core::PeriodicScheduler &scheduler);

      private:

        /**
         * @brief Synchronize S3 object between filesystem and database.
         */
        [[maybe_unused]] void SyncObjects() const;

        /**
         * @brief Synchronize S3 bucket size and object counts
         */
        [[maybe_unused]] void SyncBuckets() const;

        /**
         * Update counters
         */
        void UpdateCounter() const;

        /**
         * Metric service
         */
        Monitoring::MetricService &_metricService = Monitoring::MetricService::instance();

        /**
         * Database connection
         */
        Database::S3Database &_s3Database = Database::S3Database::instance();

        /**
         * Monitoring period
         */
        int _monitoringPeriod;

        /**
         * Worker directory object synchronization period
         */
        int _syncPeriod;

        /**
         * Worker bucket size period
         */
        int _sizePeriod;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_S3_SERVER_H
