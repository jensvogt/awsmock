//
// Created by vogje01 on 29/05/2023.
//

#include <awsmock/repository/monitoring/MonitoringMemoryRepository.h>

namespace Awsmock::Database {

    MonitoringMemoryRepository::MonitoringMemoryRepository() {
        _collectorConnection = Core::EventBus::instance().sigCollector.connect([this](const std::map<std::string, double> &values) {
            this->UpdateMonitoringCounters(values);
        });
    }

    std::vector<std::string> MonitoringMemoryRepository::GetDistinctLabelValues(const std::string &name, const std::string &labelName, const long limit, const system_clock::time_point start, const system_clock::time_point end) const {
        log_trace << "Get distinct label values, labelName: " << labelName;
        return {};
    }

    std::vector<Entity::Monitoring::Counter> MonitoringMemoryRepository::GetMonitoringValues(const std::string &name, const system_clock::time_point start, const system_clock::time_point end, const long step, const std::string &labelName,
                                                                                             const std::string &labelValue, const long limit) const {
        log_trace << "Performance counter not available if you running the memory DB";
        return {};
    }

    void MonitoringMemoryRepository::UpdateMonitoringCounters(const std::map<std::string, double> &values) const {
    }

    long MonitoringMemoryRepository::DeleteOldMonitoringData(const int retentionPeriod) const {
        log_trace << "Deleting old monitoring data, retention:: " << retentionPeriod;
        return 0;
    }

} // namespace Awsmock::Database
