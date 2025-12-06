//
// Created by vogje01 on 07/01/2023.
//

#ifndef AWSMOCK_MONITORING_METRIC_SERVICE_H
#define AWSMOCK_MONITORING_METRIC_SERVICE_H

// C++ Standard includes
#include <chrono>
#include <string>

// Prometheus
#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

// Boost include
#include <boost/thread/mutex.hpp>

// AwsMock utils
#include <awsmock/repository/MonitoringDatabase.h>
//#include <awsmock/service/monitoring/MetricCacheService.h>

namespace AwsMock::Monitoring {

    using std::chrono::high_resolution_clock;
    using std::chrono::time_point;

    typedef std::map<std::string, prometheus::Family<prometheus::Counter> *> CounterMap;
    typedef std::map<std::string, prometheus::Family<prometheus::Gauge> *> GaugeMap;
    typedef std::map<std::string, prometheus::Family<prometheus::Histogram> *> HistogramMap;

    /**
     * @brief Monitoring server
     *
     * @par
     * Maintains a list of counter and gauges for monitoring via Prometheus.
     * The data is made available via an HTTP manager listening on port 9100.
     * The port is configurable.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MetricService {

      public:

        /**
         * @brief Default constructor
         */
        MetricService();

        /**
         * @brief Singleton instance
         */
        static MetricService &instance() {
            static MetricService metricService;
            return metricService;
        }

        /**
         * @brief Initialization
         */
        void Initialize();

        /**
         * @brief Increments a counter.
         *
         * @param name of the counter
         * @param labelName name of the label
         * @param labelValue label value of the counter
         * @param value value for the incrementation (default: 1) can be negative
         */
        void IncrementCounter(const std::string &name, const std::string &labelName = {}, const std::string &labelValue = {}, int value = 1);

        /**
         * @brief Clears a counter.
         *
         * @param name of the counter
         * @param labelName name of the label
         * @param labelValue label value of the counter
         */
        void ClearCounter(const std::string &name, const std::string &labelName = {}, const std::string &labelValue = {}) const;

        /**
         * @brief Sets a double gauge value in the map.
         *
         * @param name name of the gauge
         * @param labelName label name of the gauge
         * @param labelValue label value of the gauge
         * @param value value of the gauge
         */
        [[maybe_unused]] void SetGauge(const std::string &name, const std::string &labelName = {}, const std::string &labelValue = {}, double value = 0.0);

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
         * @return true if the counter exists.
         */
        [[nodiscard]] bool CounterExists(const std::string &name) const;

        /**
         * @brief Check whether a counter exists
         *
         * @param name name of the counter.
         * @param labelName label name of the counter
         * @param labelValue label value of the counter
         * @return true if the counter exists.
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
         * @brief Adds a gauge to the map.
         *
         * @param name name of the gauge
         */
        void AddGauge(const std::string &name);

        /**
         * @brief Adds a gauge to the map.
         *
         * @param name name of the gauge
         * @param labelName name of the label
         * @param labelValue label value of the counter
         */
        void AddGauge(const std::string &name, const std::string &labelName, const std::string &labelValue);

        /**
         * @brief Adds a gauge to the map.
         *
         * @param name name of the gauge
         */
        void DoAddGauge(const std::string &name);

        /**
         * @brief Check whether a gauge exists
         *
         * @param name name of the gauge.
         * @return true if gauge exists.
         */
        [[nodiscard]] bool GaugeExists(const std::string &name) const;

        /**
         * @brief Check whether a gauge exists
         *
         * @param name name of the gauge.
         * @param labelName name of the label
         * @param labelValue label value of the counter
         * @return true if gauge exists.
         */
        bool GaugeExists(const std::string &name, const std::string &labelName, const std::string &labelValue);

        /**
         * @brief Get a specific metric
         *
         * @param name name of the metric
         * @return metric if existing.
         */
        prometheus::Family<prometheus::Gauge> *GetGauge(const std::string &name);

        /**
         * Metric manager for Prometheus
         */
        std::shared_ptr<prometheus::Exposer> _server;

        /**
         * Metric manager for Prometheus
         */
        std::shared_ptr<prometheus::Registry> _registry;

        /**
         * Counter map
         */
        CounterMap _counterMap;

        /**
         * Counter map
         */
        GaugeMap _gaugeMap;

        /**
         * Counter map
         */
        HistogramMap _histogramMap;

        /**
         * Port for the monitoring manager
         */
        long _port;

        /**
         * Prometheus flag
         */
        bool _prometheus;

        /**
         * Intern flag
         */
        bool _intern;

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

        /**
         * Database connection
         */
        //MetricCacheService _metricCacheService;
    };

}// namespace AwsMock::Monitoring

#endif// AWSMOCK_MONITORING_METRIC_SERVICE_H
