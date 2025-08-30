//
// Created by vogje01 on 12/19/24.
//

#include "awsmock/core/config/Configuration.h"


#include <awsmock/core/monitoring/SharedMemoryUtils.h>

namespace AwsMock::Core {

    boost::mutex SharedMemoryUtils::_monitoringMutex;

    SharedMemoryUtils::SharedMemoryUtils() {

        try {
            _segment = {boost::interprocess::open_only, MONITORING_SEGMENT_NAME};
        } catch (...) {
            // Ensure a clean state
            auto sharedMemorySize = Configuration::instance().GetValue<unsigned long>("awsmock.shm-size");
            boost::interprocess::shared_memory_object::remove(MONITORING_SEGMENT_NAME);
            _segment = {boost::interprocess::create_only, MONITORING_SEGMENT_NAME, sharedMemorySize};
        }
        MapAllocator map_alloc(_segment.get_segment_manager());

        // Construct the map in shared memory
        _counterMap = _segment.construct<ShmemMap>(MONITORING_MAP_NAME)(std::less<>(), map_alloc);
        log_debug << "Counter map initialized, name: " << std::string(MONITORING_MAP_NAME);
    }

    void SharedMemoryUtils::SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) const {
        boost::mutex::scoped_lock lock(_monitoringMutex);

        const ShmemAllocator<char> char_alloc(_segment.get_segment_manager());

        if (ShmemString k(GetId(name, labelName, labelValue).c_str(), char_alloc); _counterMap->find(k) == _counterMap->end()) {
            MonitoringCounter v(char_alloc);
            v.name = ShmemString(name.c_str(), char_alloc);
            v.labelName = ShmemString(labelName.c_str(), char_alloc);
            v.labelValue = ShmemString(labelValue.c_str(), char_alloc);
            v.type = GAUGE;
            v.count = 1;
            v.value = value;
            _counterMap->emplace(boost::move(k), boost::move(v));
        } else {
            MonitoringCounter counter = _counterMap->at(k);
            counter.count++;
            counter.value += value;
            counter.timestamp = system_clock::now();
            _counterMap->emplace(std::move(k), std::move(counter));
        }
    }

    void SharedMemoryUtils::SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, const long value) const {
        SetGauge(name, labelName, labelValue, static_cast<double>(value));
    }

    void SharedMemoryUtils::SetGauge(const std::string &name, const double value) const {
        boost::mutex::scoped_lock lock(_monitoringMutex);

        const ShmemAllocator<char> char_alloc(_segment.get_segment_manager());

        if (ShmemString k(GetId(name, {}, {}).c_str(), char_alloc); _counterMap->find(k) == _counterMap->end()) {
            MonitoringCounter v(char_alloc);
            v.name = ShmemString(name.c_str(), char_alloc);
            v.type = GAUGE;
            v.count = 1;
            v.value = value;
            _counterMap->emplace(boost::move(k), boost::move(v));
        } else {
            MonitoringCounter counter = _counterMap->at(k);
            counter.count++;
            counter.value += value;
            counter.timestamp = system_clock::now();
            _counterMap->emplace(std::move(k), std::move(counter));
        }
    }

    void SharedMemoryUtils::SetGauge(const std::string &name, const long value) const {
        SetGauge(name, static_cast<double>(value));
    }

    void SharedMemoryUtils::IncGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) const {
        boost::mutex::scoped_lock lock(_monitoringMutex);

        const ShmemAllocator<char> char_alloc(_segment.get_segment_manager());

        if (ShmemString k(GetId(name, labelName, labelValue).c_str(), char_alloc); _counterMap->find(k) != _counterMap->end()) {
            MonitoringCounter counter = _counterMap->at(k);
            counter.count++;
            counter.value += value;
            counter.timestamp = system_clock::now();
            _counterMap->emplace(std::move(k), std::move(counter));
        }
    }

    void SharedMemoryUtils::IncGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, const long value) const {
        IncGauge(name, labelName, labelValue, static_cast<double>(value));
    }

    void SharedMemoryUtils::IncrementCounter(const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) const {
        boost::mutex::scoped_lock lock(_monitoringMutex);

        const ShmemAllocator<char> char_alloc(_segment.get_segment_manager());

        if (ShmemString k(GetId(name, labelName, labelValue).c_str(), char_alloc); _counterMap->find(k) == _counterMap->end()) {
            MonitoringCounter v(char_alloc);
            v.name = ShmemString(name.c_str(), char_alloc);
            v.labelName = ShmemString(labelName.c_str(), char_alloc);
            v.labelValue = ShmemString(labelValue.c_str(), char_alloc);
            v.type = COUNTER;
            v.count = 1;
            v.value = value;
            _counterMap->emplace(boost::move(k), boost::move(v));
            log_info << "Free SHM: " << _segment.get_free_memory();
        } else {
            MonitoringCounter counter = _counterMap->at(k);
            counter.count++;
            counter.value += value;
            counter.timestamp = system_clock::now();
            _counterMap->emplace(std::move(k), std::move(counter));
        }
    }

    long SharedMemoryUtils::Size() const {
        const auto total = Configuration::instance().GetValue<unsigned long>("awsmock.shm-size");
        return static_cast<long>(total - _segment.get_free_memory());
    }

    ShmemMap *SharedMemoryUtils::GetCounterMap() const {
        return _counterMap;
    }

    void SharedMemoryUtils::Clear(const std::string &name) const {
        for (const auto &key: *_counterMap | std::views::keys) {
            if (StringUtils::StartsWith(key.data(), name)) {
                _counterMap->erase(key);
            }
        }
    }

    void SharedMemoryUtils::Clear(const std::string &name, const std::string &labelName, const std::string &labelValue) const {
        _counterMap->erase(GetId(name, labelName, labelValue));
    }

    void SharedMemoryUtils::Clear() const {
        _counterMap->clear();
    }

    std::string SharedMemoryUtils::GetId(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        if (labelName.empty()) {
            return name;
        }
        return name + ":" + labelName + ":" + labelValue;
    }

    std::string SharedMemoryUtils::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const SharedMemoryUtils &s) {
        const auto sharedMemorySize = Configuration::instance().GetValue<unsigned long>("awsmock.shm-size");
        os << "SharedMemory: " << s.Size() << "/" << sharedMemorySize;
        return os;
    }

}// namespace AwsMock::Core
