//
// Created by vogje01 on 10/2/24.
//

#pragma once

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/EventBus.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/entity/monitoring/Counter.h>
#include <awsmock/repository/monitoring/IMonitoringRepository.h>

namespace Awsmock::Database {

    /**
     * @brief Performance MongoDB database.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MonitoringMemoryRepository final : public IMonitoringRepository {

    public:
        /**
         * @brief Constructor
         */
        explicit MonitoringMemoryRepository();

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
        std::vector<Entity::Monitoring::Counter> getMonitoringValues(const std::string &name, system_clock::time_point start, system_clock::time_point end, long step, const std::string &labelName, const std::string &labelValue,
                                                                     long limit) const override;

        /**
         * @brief Saves the monitoring data to the database
         *
         * @param values key value map of values
         */
        void updateMonitoringCounters(const std::map<std::string, double> &values) const override;

        /**
         * @brief Returns list of label values by label name
         *
         * @param name monitoring feature name
         * @param labelName label name
         * @param limit total number of labels
         * @param start start timestamp
         * @param end end timestamp
         * @return list of label values
         */
        [[nodiscard]]
        std::vector<std::string> getDistinctLabelValues(const std::string &name, const std::string &labelName, long limit, system_clock::time_point start, system_clock::time_point end) const override;

        /**
         * @brief Deletes old monitoring data
         *
         * @param retentionPeriod retention period in days
         * @return number of deleted data rows
         */
        [[nodiscard]]
        long deleteOldMonitoringData(int retentionPeriod) const override;

    private:
        /**
         * @brief Channeled logger
         */
        mutable logger_t _logger{boost::log::keywords::channel = "Monitoring"};

        /**
         * Scoped signal connection — auto-disconnects on destruction.
         */
        boost::signals2::scoped_connection _collectorConnection;

        /**
         * @brief Decompose the monitoring ID
         *
         * @param id monitoring ID
         * @param name metric name
         * @param labelName metric label name
         * @param labelValue metric label value
         */
        static void GetIdValues(const std::string &id, std::string &name, std::string &labelName, std::string &labelValue);
    };

    inline void MonitoringMemoryRepository::GetIdValues(const std::string &id, std::string &name, std::string &labelName, std::string &labelValue) {
        if (std::vector<std::string> keys = Core::StringUtils::Split(id, ":"); keys.size() == 1) {
            name = std::move(keys[0]);
        } else {
            name = std::move(keys[0]);
            labelName = std::move(keys[1]);
            labelValue = std::move(keys[2]);
        }
    }
} // namespace Awsmock::Database
