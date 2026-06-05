//
// Created by vogje01 on 5/24/26.
//

#pragma once

// C++ includes
#include <string>
#include <vector>

// Awsmock includes
#include <awsmock/entity/monitoring/Counter.h>
#include <awsmock/utils/SortColumn.h>

namespace Awsmock::Database {

    /**
     * @brief Interface for monitoring repository operations.
     *
     * Provides an abstraction for storing, retrieving, and managing
     * monitoring-related data.
     */
    class IMonitoringRepository {

      public:

        /**
         * @brief Virtual destructor for the IMonitoringRepository interface.
         *
         * Ensures derived classes' destructor is invoked correctly
         * during object destruction to release resources.
         */
        virtual ~IMonitoringRepository() = default;

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
         * @return list of counter-values
         */
        [[nodiscard]]
        virtual std::vector<Entity::Monitoring::Counter> getMonitoringValues(const std::string &name, system_clock::time_point start, system_clock::time_point end, long step, const std::string &labelName, const std::string &labelValue, long limit) const = 0;

        /**
         * @brief Saves the monitoring data to the database
         *
         * @param values key value map of values
         */
        virtual void updateMonitoringCounters(const std::map<std::string, double> &values) const = 0;

        /**
         * @brief Returns a list of label values by label name
         *
         * @param name monitoring feature name
         * @param labelName label name
         * @param limit total number of labels
         * @param start start timestamp
         * @param end end timestamp
         * @return list of label values
         */
        [[nodiscard]]
        virtual std::vector<std::string> getDistinctLabelValues(const std::string &name, const std::string &labelName, long limit, system_clock::time_point start, system_clock::time_point end) const = 0;

        /**
         * @brief Deletes old monitoring data
         *
         * @param retentionPeriod retention period in days
         * @return number of deleted data rows
         */
        [[nodiscard]]
        virtual long deleteOldMonitoringData(int retentionPeriod) const = 0;
    };

}// namespace Awsmock::Database
