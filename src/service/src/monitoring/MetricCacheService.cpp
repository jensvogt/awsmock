//
// Created by vogje01 on 07/01/2023.
//

#include <awsmock/service/monitoring/MetricCacheService.h>

namespace AwsMock::Monitoring {

    boost::mutex MetricCacheService::_cacheMutex;

    MetricCacheService::MetricCacheService() : _aggregationPeriod(300), _database(Database::MonitoringDatabase::instance()) {}

    void MetricCacheService::ClearCounter(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        boost::mutex::scoped_lock lock(_cacheMutex);

        const auto count = std::erase_if(_metricCache, [this, name, labelName, labelValue](const auto &item) {
            auto const &[key, value] = item;
            return key == GetId(name, labelName, labelValue);
        });
        log_trace << "Counter cleared, count: " << count;
    }

    void MetricCacheService::IncrementCounter(const std::string &name, const int value, const std::string &labelName, const std::string &labelValue) {
        boost::mutex::scoped_lock lock(_cacheMutex);

        const auto it = std::ranges::find_if(_metricCache, [this, name, labelName, labelValue](const auto &item) {
            auto const &[k, v] = item;
            return k == GetId(name, labelName, labelValue);
        });

        if (it != _metricCache.end()) {
            it->second.value += value;
            it->second.count++;
            if (duration_cast<minutes>(system_clock::now() - it->second.lastWritten).count() > 1) {
                _database.IncCounter(name, it->second.value / it->second.count, labelName, labelValue);
                it->second.lastWritten = system_clock::now();
                it->second.value = 0;
                it->second.count = 0;
            }
        } else {
            _metricCache[GetId(name, labelName, labelValue)] = {.name = name, .labelName = labelName, .labelValue = labelValue, .value = static_cast<double>(value), .count = 1};
        }
        log_trace << "Counter incremented, name: " << name << " labelName: " << labelName << " labelValue: " << labelValue;
    }

    void MetricCacheService::SetGauge(const std::string &name, const double value, const std::string &labelName, const std::string &labelValue) {
        boost::mutex::scoped_lock lock(_cacheMutex);

        const auto it = std::ranges::find_if(_metricCache, [this, name, labelName, labelValue](const auto &item) {
            auto const &[k, v] = item;
            return k == GetId(name, labelName, labelValue);
        });

        if (it != _metricCache.end()) {
            it->second.value += value;
            it->second.count++;
            if (duration_cast<minutes>(system_clock::now() - it->second.lastWritten).count() > 1) {
                _database.SetGauge(name, it->second.value / it->second.count, labelName, labelValue);
                it->second.lastWritten = system_clock::now();
                it->second.value = 0;
                it->second.count = 0;
            }
        } else {
            _metricCache[GetId(name, labelName, labelValue)] = {.name = name, .labelName = labelName, .labelValue = labelValue, .value = value, .count = 1};
        }
    }

    std::string MetricCacheService::GetId(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        if (labelName.empty()) {
            return name;
        }
        return name + ":" + labelName + ":" + labelValue;
    }
}// namespace AwsMock::Monitoring