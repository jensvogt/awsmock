//
// Created by jensv on 17/03/2026.
//

#pragma once

// C++ includes
#include <map>
#include <mutex>
#include <string>

// Boost includes
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>

namespace Awsmock::Monitoring {

    class MonitoringCollector {
      public:

        /**
         * @brief Constructor
         */
        MonitoringCollector() {
            _period = Core::Configuration::instance().get<long>("awsmock.monitoring.average-period");
        }

        /**
         * @brief Process accumulated metrics and emit them via EventBus.
         *
         * Called periodically by Core::Scheduler.
         */
        void Collect() {
            std::map<std::string, double> values;
            {
                std::lock_guard lock(_monitoringMutex);
                for (auto &[id, entry]: _collectorMap) {
                    if (const auto cnt = boost::accumulators::count(entry.accumulator); cnt > 0) {
                        if (entry.isRate) {
                            values[id] = static_cast<double>(cnt) / static_cast<double>(_period);
                        } else {
                            values[id] = boost::accumulators::mean(entry.accumulator);
                        }
                        entry.accumulator = {};
                    }
                }
                std::erase_if(_collectorMap, [](const auto &kv) {
                    return boost::accumulators::count(kv.second.accumulator) == 0;
                });
            }

            if (!values.empty()) {
                Core::EventBus::instance().sigCollector(values);
            }
        }

        void SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) {
            std::lock_guard lock(_monitoringMutex);
            auto &[accumulator, isRate] = _collectorMap[GetId(name, labelName, labelValue)];
            isRate = false;
            accumulator(value);
        }

        void Increment(const std::string &name, const std::string &labelName, const std::string &labelValue) {
            std::lock_guard lock(_monitoringMutex);
            auto &[accumulator, isRate] = _collectorMap[GetId(name, labelName, labelValue)];
            isRate = true;
            accumulator(1.0);
        }

      private:

        struct MetricEntry {
            boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::mean, boost::accumulators::tag::count>> accumulator;
            bool isRate = false;
        };

        /**
         * @brief Creates a metric ID
         */
        static std::string GetId(const std::string &name, const std::string &labelName, const std::string &labelValue);

        std::map<std::string, MetricEntry> _collectorMap;
        std::mutex _monitoringMutex;

        /**
         * @brief Average period in seconds — used for rate normalisation.
         */
        long _period{};
    };

    inline std::string MonitoringCollector::GetId(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        if (labelName.empty()) {
            return name;
        }
        return name + ":" + labelName + ":" + labelValue;
    }

}// namespace Awsmock::Monitoring
