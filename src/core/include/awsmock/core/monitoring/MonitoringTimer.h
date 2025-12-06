//
// Created by vogje01 on 18/05/2023.
//

#ifndef AWSMOCK_MONITORING_METRIC_SERVICE_TIMER_H
#define AWSMOCK_MONITORING_METRIC_SERVICE_TIMER_H

// C++ standard includes
#include <chrono>
#include <utility>

// Awsmock include
#include <awsmock/core/monitoring/MonitoringCollector.h>
#include <awsmock/core/monitoring/MonitoringDefinition.h>

#define TIME_DIFF (duration_cast<std::chrono::milliseconds>(system_clock::now() - _start).count())

namespace AwsMock::Monitoring {

    using std::chrono::system_clock;

    /**
     * @brief Measures the execution time of a method.
     *
     * @par
     * Is self-destroying, which means the timer is automatically destroyed when the method goes out of scope.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class MonitoringTimer {

      public:

        /**
         * @brief Constructor
         *
         * @par
         * Measures a method execution time.</p>
         *
         * @param name name of the underlying timer
         */
        explicit MonitoringTimer(std::string name) : _shmUtils(Core::MonitoringCollector::instance()), _timerName(std::move(name)), _start(system_clock::now()) {}

        /**
         * @brief Constructor
         *
         * @par
         * Measures a method execution time.</p>
         *
         * @param name name of the underlying timer
         * @param labelName label name of the underlying timer
         * @param labelValue label value of the underlying timer
         */
        explicit MonitoringTimer(std::string name, std::string labelName, std::string labelValue) : _shmUtils(Core::MonitoringCollector::instance()), _timerName(std::move(name)),
                                                                                                    _labelName(std::move(labelName)), _labelValue(std::move(labelValue)), _start(system_clock::now()) {}

        /**
         * @brief Constructor
         *
         * @par
         * Measures a method execution time.</p>
         *
         * @param timerName name of the underlying timer
         * @param counterName name of the corresponding counter
         * @param labelName label name of the underlying timer
         * @param labelValue label value of the underlying timer
         */
        explicit MonitoringTimer(std::string timerName, std::string counterName, std::string labelName, std::string labelValue) : _shmUtils(Core::MonitoringCollector::instance()),
                                                                                                                                  _timerName(std::move(timerName)), _counterName(std::move(counterName)),
                                                                                                                                  _labelName(std::move(labelName)), _labelValue(std::move(labelValue)), _start(system_clock::now()) {}

        /**
         * @brief Destructor
         *
         * @par
         * Stop the timer and reports the execution to the metric service.
         */
        ~MonitoringTimer() {
            if (!_counterName.empty()) {
                _shmUtils.IncCountPerSec(_counterName, _labelName, _labelValue, 1.0);
            }
            if (_labelName.empty()) {
                _shmUtils.SetGauge(_timerName, static_cast<double>(TIME_DIFF));
                log_trace << "Timer deleted, name: " << _timerName;
            } else {
                _shmUtils.SetGauge(_timerName, _labelName, _labelValue, static_cast<double>(TIME_DIFF));
                log_trace << "Timer deleted, name: " << _timerName << " labelName: " << _labelName << " labelValue: " << _labelValue;
            }
        }

        /**
         * Default constructor
         */
        MonitoringTimer() = delete;

        /**
         * Copy constructor
         */
        MonitoringTimer(const MonitoringTimer &) = delete;

        /**
         * Equals operator
         */
        MonitoringTimer &operator=(const MonitoringTimer &) = delete;

      private:

        /**
         * Metric module
         */
        Core::MonitoringCollector &_shmUtils;

        /**
         * Name of the timer
         */
        std::string _timerName{};

        /**
         * Name of the counter
         */
        std::string _counterName{};

        /**
         * Label name of the timer
         */
        std::string _labelName{};

        /**
         * Label value of the timer
         */
        std::string _labelValue{};

        /**
         * Timer start time point
         */
        system_clock::time_point _start;
    };

}// namespace AwsMock::Monitoring

#endif//AWSMOCK_MONITORING_METRIC_SERVICE_TIMER_H
