//
// Created by vogje01 on 12/19/24.
//

#include <awsmock/core/monitoring/SharedMemoryUtils.h>

namespace AwsMock::Core {

    boost::mutex SharedMemoryUtils::_monitoringMutex;

    SharedMemoryUtils::SharedMemoryUtils() {

        // Shared memory segment
        _segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, MONITORING_SEGMENT_NAME);

        // Map of monitoring counters
        _counterMap = _segment.find<CounterMapType>(MONITORING_MAP_NAME).first;
        if (!_counterMap) {
            _counterMap = _segment.construct<CounterMapType>(MONITORING_MAP_NAME)(std::less<std::string>(), _segment.get_segment_manager());
        }
        log_debug << "Counter map initialized, name: " << std::string(MONITORING_MAP_NAME);
    }

    void SharedMemoryUtils::SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) const {
        boost::mutex::scoped_lock lock(_monitoringMutex);

        if (const std::string counterKey = GetId(name, labelName, labelValue); _counterMap->find(counterKey) == _counterMap->end()) {
            MonitoringCounter counter = {name, labelName, labelValue, value, 1, GAUGE, system_clock::now()};
            _counterMap->emplace(counterKey, counter);
        } else {
            MonitoringCounter counter = _counterMap->at(counterKey);
            counter.count++;
            counter.value += value;
            counter.timestamp = system_clock::now();
            _counterMap->emplace(counterKey, std::move(counter));
        }
    }

    void SharedMemoryUtils::IncrementCounter(const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) const {
        boost::mutex::scoped_lock lock(_monitoringMutex);
        if (const std::string counterKey = GetId(name, labelName, labelValue); _counterMap->find(counterKey) != _counterMap->end()) {
            MonitoringCounter counter = {name, labelName, labelValue, value, 1, COUNTER, system_clock::now()};
            _counterMap->emplace(counterKey, counter);
        } else {
            MonitoringCounter counter = _counterMap->at(counterKey);
            counter.count++;
            counter.value += value;
            counter.timestamp = system_clock::now();
            _counterMap->emplace(counterKey, std::move(counter));
        }
    }

    long SharedMemoryUtils::Size() const {
        return static_cast<long>(_counterMap->size());
    }

    SharedMemoryUtils::CounterMapType *SharedMemoryUtils::GetCounterMap() const {
        return _counterMap;
    }

    std::string SharedMemoryUtils::GetId(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        if (labelName.empty()) {
            return name;
        }
        return name + ":" + labelName + ":" + labelValue;
    }

}// namespace AwsMock::Core
