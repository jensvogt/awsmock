//
// Created by vogje01 on 12/19/24.
//

#include <awsmock/core/monitoring/MonitoringCollector.h>

namespace AwsMock::Core {

    boost::mutex MonitoringCollector::_monitoringMutex;

    MonitoringCollector::MonitoringCollector() {

        _period = Configuration::instance().GetValue<long>("awsmock.monitoring.average-period");

        try {
            _segment = {boost::interprocess::open_only, MONITORING_SEGMENT_NAME};
        } catch (...) {
            // Ensure a clean state
            auto sharedMemorySize = Configuration::instance().GetValue<unsigned long>("awsmock.shm-size");
            boost::interprocess::shared_memory_object::remove(MONITORING_SEGMENT_NAME);
            _segment = {boost::interprocess::create_only, MONITORING_SEGMENT_NAME, sharedMemorySize};
        }

        // Construct the map in shared memory
        _counterMap = _segment.construct<ShmMap>(MONITORING_MAP_NAME)(std::less<>(), _segment.get_segment_manager());
        log_debug << "Counter map initialized, name: " << std::string(MONITORING_MAP_NAME);
    }

    void MonitoringCollector::SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) const {
        boost::mutex::scoped_lock lock(_monitoringMutex);

        const ShmAllocator<char> char_alloc(_segment.get_segment_manager());

        if (ShmString k(GetId(name, labelName, labelValue).c_str(), char_alloc); _counterMap->find(k) == _counterMap->end()) {
            MonitoringCounter v(char_alloc);
            v.name = ShmString(name.c_str(), char_alloc);
            v.labelName = ShmString(labelName.c_str(), char_alloc);
            v.labelValue = ShmString(labelValue.c_str(), char_alloc);
            v.type = GAUGE;
            v.count = 1;
            v.value = value;
            v.timestamp = system_clock::now();
            _counterMap->emplace(boost::move(k), boost::move(v));
        } else {
            _counterMap->at(k).count++;
            _counterMap->at(k).value += value;
            _counterMap->at(k).timestamp = system_clock::now();
        }
    }

    void MonitoringCollector::SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, const long value) const {
        SetGauge(name, labelName, labelValue, static_cast<double>(value));
    }

    void MonitoringCollector::SetGauge(const std::string &name, const double value) const {
        SetGauge(name, {}, {}, value);
    }

    void MonitoringCollector::SetGauge(const std::string &name, const long value) const {
        SetGauge(name, static_cast<double>(value));
    }

    void MonitoringCollector::IncCountPerSec(const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) const {
        boost::mutex::scoped_lock lock(_monitoringMutex);

        const ShmAllocator<char> char_alloc(_segment.get_segment_manager());
        if (ShmString k(GetId(name, labelName, labelValue).c_str(), char_alloc); _counterMap->find(k) == _counterMap->end()) {
            MonitoringCounter v(char_alloc);
            v.name = ShmString(name.c_str(), char_alloc);
            v.labelName = ShmString(labelName.c_str(), char_alloc);
            v.labelValue = ShmString(labelValue.c_str(), char_alloc);
            v.type = COUNT_PER_SECOND;
            v.count = _period;
            v.value += value;
            v.timestamp = system_clock::now();
            _counterMap->emplace(boost::move(k), boost::move(v));
        } else {
            _counterMap->at(k).value += value;
            _counterMap->at(k).timestamp = system_clock::now();
        }
    }

    void MonitoringCollector::IncCountAbs(const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) const {
        boost::mutex::scoped_lock lock(_monitoringMutex);

        const ShmAllocator<char> char_alloc(_segment.get_segment_manager());
        if (ShmString k(GetId(name, labelName, labelValue).c_str(), char_alloc); _counterMap->find(k) == _counterMap->end()) {
            MonitoringCounter v(char_alloc);
            v.name = ShmString(name.c_str(), char_alloc);
            v.labelName = ShmString(labelName.c_str(), char_alloc);
            v.labelValue = ShmString(labelValue.c_str(), char_alloc);
            v.type = COUNT_ABSOLUTE;
            v.value += value;
            v.timestamp = system_clock::now();
            _counterMap->emplace(boost::move(k), boost::move(v));
        } else {
            _counterMap->at(k).value = value;
            _counterMap->at(k).timestamp = system_clock::now();
        }
    }

    void MonitoringCollector::IncCountAbs(const std::string &name, const double value) const {
        IncCountAbs(name, {}, {}, value);
    }

    long MonitoringCollector::Size() const {
        const auto total = Configuration::instance().GetValue<unsigned long>("awsmock.shm-size");
        return static_cast<long>(total - _segment.get_free_memory());
    }

    ShmMap *MonitoringCollector::GetCounterMap() const {
        return _counterMap;
    }

    void MonitoringCollector::DumpCounterMap() const {
        for (const auto &[fst, snd]: *_counterMap) {
            log_info << fst << ", type: " << snd.type << " value: " << snd.value << " count: " << snd.count;
        }
    }

    void MonitoringCollector::Clear(const std::string &name) const {
        for (const auto &key: *_counterMap | std::views::keys) {
            if (StringUtils::StartsWith(key.data(), name)) {
                _counterMap->erase(key);
            }
        }
    }

    void MonitoringCollector::Clear(const std::string &name, const std::string &labelName, const std::string &labelValue) const {
        const ShmAllocator<char> char_alloc(_segment.get_segment_manager());
        _counterMap->erase(ShmString(GetId(name, labelName, labelValue).data(), char_alloc));
    }

    void MonitoringCollector::Clear() const {
        _counterMap->clear();
    }

    std::string MonitoringCollector::GetId(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        if (labelName.empty()) {
            return name;
        }
        return name + ":" + labelName + ":" + labelValue;
    }

    std::string MonitoringCollector::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const MonitoringCollector &s) {
        const auto sharedMemorySize = Configuration::instance().GetValue<unsigned long>("awsmock.shm-size");
        os << "SharedMemory: " << s.Size() << "/" << sharedMemorySize << " (used/total)";
        return os;
    }

}// namespace AwsMock::Core
