//
// Created by vogje01 on 07/01/2023.
//

#include <awsmock/service/monitoring/MetricCacheService.h>

namespace AwsMock::Monitoring {

    boost::mutex MetricCacheService::_cacheMutex;

    MetricCacheService::MetricCacheService() : _database(Database::MonitoringDatabase::instance()) {
        _aggregationPeriod = Core::Configuration::instance().GetValue<int>("awsmock.monitoring.average");
        if (_aggregationPeriod < 60) {
            _aggregationPeriod = 60;
        }
        log_debug << "Metric cache service initialized, aggregation period: " << _aggregationPeriod << " seconds";
    }

    void MetricCacheService::ClearCounter(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        boost::mutex::scoped_lock lock(_cacheMutex);

        const auto count = std::erase_if(_metricCache, [name, labelName, labelValue](const auto &item) {
            return item.first == GetId(name, labelName, labelValue);
        });
        log_trace << "Counter cleared, count: " << count;
    }

    void MetricCacheService::IncrementCounter(const std::string &name, const int value, const std::string &labelName, const std::string &labelValue) {
        boost::mutex::scoped_lock lock(_cacheMutex);

        const auto it = std::ranges::find_if(_metricCache, [name, labelName, labelValue](const auto &item) {
            return item.first == GetId(name, labelName, labelValue);
        });

        if (it != _metricCache.end()) {
            it->second.value += value;
            it->second.count++;
            if (duration_cast<seconds>(system_clock::now() - it->second.lastWritten).count() > _aggregationPeriod) {
                _database.IncCounter(name, it->second.value, labelName, labelValue);
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

        const auto it = std::ranges::find_if(_metricCache, [name, labelName, labelValue](const auto &item) {
            return item.first == GetId(name, labelName, labelValue);
        });

        if (it != _metricCache.end()) {
            it->second.value += value;
            it->second.count++;
            if (duration_cast<seconds>(system_clock::now() - it->second.lastWritten).count() > _aggregationPeriod) {
                _database.SetGauge(name, it->second.value / static_cast<double>(it->second.count), labelName, labelValue);
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