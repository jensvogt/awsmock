//
// Created by vogje01 on 07/01/2023.
//

#ifndef AWSMOCK_MONITORING_CACHE_SERVICE_H
#define AWSMOCK_MONITORING_CACHE_SERVICE_H

// C++ Standard includes
#include <chrono>
#include <map>
#include <string>

// Prometheus
#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

// Boost include
#include <boost/thread/mutex.hpp>

// AwsMock utils
#include <awsmock/repository/MonitoringDatabase.h>

namespace AwsMock::Monitoring {

    using namespace std::chrono;

    struct MetricCache {
        std::string name;
        std::string labelName;
        std::string labelValue;
        double value;
        int count;
        system_clock::time_point lastWritten = system_clock::now();
    };

    /**
     * @brief Monitoring server
     *
     * Maintains a list of counter and gauges for monitoring via Prometheus. The data is made available via a HTTP
     * manager listening on port 9100. The port ist configurable.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MetricCacheService {

      public:

        /**
         * @brief Default constructor
         */
        MetricCacheService();

        /**
         * @brief Initialization
         */
        void Initialize();

        /**
         * @brief Increments a labeled counter.
         *
         * @param name of the counter
         * @param labelName name of the label
         * @param labelValue label value of the counter
         * @param value value for the incrementation (default: 1), can be negative
         */
        void IncrementCounter(const std::string &name, int value = 1, const std::string &labelName = {}, const std::string &labelValue = {});

        /**
         * @brief Clears a counter.
         *
         * @param name of the counter
         */
        void ClearCounter(const std::string &name);

        /**
         * @brief Clears a counter.
         *
         * @param name of the counter
         * @param labelName name of the label
         * @param labelValue label value of the counter
         */
        void ClearCounter(const std::string &name, const std::string &labelName, const std::string &labelValue);

        /**
         * @brief Sets a double gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        void SetGauge(const std::string &name, double value, const std::string &labelName = {}, const std::string &labelValue = {});

      private:

        /**
         * @brief Adds a counter to the map.
         *
         * @param name name of the counter
         */
        void AddCounter(const std::string &name);

        /**
         * @brief Adds a counter to the map.
         *
         * @param name name of the counter
         * @param labelName label name of the counter
         * @param labelValue label value of the counter
         */
        void AddCounter(const std::string &name, const std::string &labelName, const std::string &labelValue);

        /**
         * @brief Adds a counter to the map.
         *
         * @param name name of the counter
         */
        void DoAddCounter(const std::string &name);

        /**
         * @brief Check whether a counter exists
         *
         * @param name name of the counter.
         * @return true if counter exists.
         */
        [[nodiscard]] bool CounterExists(const std::string &name) const;

        /**
         * @brief Check whether a counter exists
         *
         * @param name name of the counter.
         * @param labelName label name of the counter
         * @param labelValue label value of the counter
         * @return true if counter exists.
         */
        [[nodiscard]] bool CounterExists(const std::string &name, const std::string &labelName, const std::string &labelValue) const;

        /**
         * @brief Get a specific metric
         *
         * @param name name of the metric
         * @return metric if existing.
         */
        [[nodiscard]] prometheus::Family<prometheus::Counter> *GetCounter(const std::string &name) const;

        /**
         * @brief Returns the cache primary key
         *
         * @param name name of the counter.
         * @param labelName label name of the counter
         * @param labelValue label value of the counter
         * @return cache primary key
         */
        static std::string GetId(const std::string &name, const std::string &labelName, const std::string &labelValue);

        /**
         * Mutex
         */
        static boost::mutex _gaugeMutex;

        /**
         * Mutex
         */
        static boost::mutex _counterMutex;

        /**
         * Database
         */
        Database::MonitoringDatabase &_database;

        std::map<std::string, MetricCache> _metricCache;
    };

}// namespace AwsMock::Monitoring

#endif// AWSMOCK_MONITORING_CACHE_SERVICE_H
