//
// Created by vogje01 on 10/2/24.
//

#ifndef AWSMOCK_REPOSITORY_PERFORMANCE_DATABASE_H
#define AWSMOCK_REPOSITORY_PERFORMANCE_DATABASE_H

// C++ standard includes
#include <string>

// Boost includes
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/exception/DatabaseException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/monitoring/MonitoringCollector.h>
#include <awsmock/entity/monitoring/Counter.h>
#include <awsmock/repository/Database.h>

namespace AwsMock::Database {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_array;
    using bsoncxx::builder::basic::make_document;
    using std::chrono::system_clock;

    /**
     * @brief Performance MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MonitoringDatabase : public DatabaseBase {

      public:

        /**
         * @brief Constructor
         */
        explicit MonitoringDatabase();

        /**
         * @brief Singleton instance
         */
        static MonitoringDatabase &instance() {
            static MonitoringDatabase monitoringDatabase;
            return monitoringDatabase;
        }

        /**
         * @brief Increments a metric counter
         *
         * @param name metric counter name
         * @param labelName metric label name
         * @param labelValue metric label name
         * @param value metric value
         */
        void IncCounter(const std::string &name, double value, const std::string &labelName = {}, const std::string &labelValue = {}) const;

        /**
         * @brief Sets a metric gauge
         *
         * @param name metric counter name
         * @param labelName metric label name
         * @param labelValue metric label name
         * @param value metric value
         */
        void SetGauge(const std::string &name, double value, const std::string &labelName = {}, const std::string &labelValue = {}) const;

        /**
         * @brief Returns the rolling mean
         *
         * @param name counter name
         * @param start start timestamp
         * @param end stop timestamp
         * @param step steps
         * @param labelName label name
         * @param labelValue labelValue
         * @param limit value limit
         * @return list of counter values
         */
        [[nodiscard]] std::vector<Entity::Monitoring::Counter> GetMonitoringValues(const std::string &name, system_clock::time_point start, system_clock::time_point end, long step, const std::string &labelName = {}, const std::string &labelValue = {}, long limit = 10) const;

        void UpdateMonitoringCounters();

        /**
         * @brief Returns list of label values by label name
         *
         * @param name monitoring feature name
         * @param labelName label name
         * @param limit total number of labels
         * @return list of label values
         */
        [[nodiscard]] std::vector<std::string> GetDistinctLabelValues(const std::string &name, const std::string &labelName, long limit = -1) const;

        /**
         * @brief Saves the monitoring data to the database
         */
        void UpdateMonitoringCounters() const;

        /**
         * @brief Deletes old monitoring data
         *
         * @param retentionPeriod retention period in days
         * @return number of deleted data rows
         */
        [[nodiscard]] long DeleteOldMonitoringData(int retentionPeriod) const;

      private:

        /**
         * Database name
         */
        std::string _databaseName;

        /**
         * Performance collection name
         */
        std::string _monitoringCollectionName;

        /**
         * Use rolling mean
         */
        bool _rollingMean;
    };

}// namespace AwsMock::Database

#endif//AWSMOCK_REPOSITORY_PERFORMANCE_DATABASE_H
