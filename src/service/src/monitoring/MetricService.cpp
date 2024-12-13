//
// Created by vogje01 on 07/01/2023.
//

#include <awsmock/service/monitoring/MetricService.h>

namespace AwsMock::Monitoring {

    boost::mutex MetricService::_gaugeMutex;
    boost::mutex MetricService::_counterMutex;

    MetricService::MetricService() : _database(Database::MonitoringDatabase::instance()) {

        const Core::Configuration &configuration = Core::Configuration::instance();
        _port = configuration.GetValueInt("awsmock.monitoring.port");
    }

    void MetricService::Initialize() {
        std::string bindAddress = "localhost:" + std::to_string(_port);
        _server = std::make_shared<prometheus::Exposer>(bindAddress);
        _registry = std::make_shared<prometheus::Registry>();
        _server->RegisterCollectable(_registry);
        log_debug << "Monitoring initialized";
    }

    void MetricService::AddCounter(const std::string &name) {
        boost::mutex::scoped_lock lock(_counterMutex);
        DoAddCounter(name);
    }

    void MetricService::DoAddCounter(const std::string &name) {
        try {
            auto &counter = prometheus::BuildCounter()
                                    .Name(name)
                                    .Help(name)
                                    .Register(*_registry);
            _counterMap[name] = &counter;
            log_trace << "Counter added, name: " << name;
        } catch (std::exception &e) {
            log_error << e.what();
        }
    }
    void MetricService::AddCounter(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        boost::mutex::scoped_lock lock(_counterMutex);
        try {
            if (GetCounter(name) == nullptr) {
                DoAddCounter(name);
            }
            if (!GetCounter(name)->Has({{labelName, labelValue}})) {
                GetCounter(name)->Add({{labelName, labelValue}});
            }
        } catch (std::exception &e) {
            log_error << e.what();
        }
    }

    bool MetricService::CounterExists(const std::string &name) const {
        return _counterMap.contains(name);
    }

    bool MetricService::CounterExists(const std::string &name, const std::string &labelName, const std::string &labelValue) const {
        return _counterMap.contains(name) && GetCounter(name)->Has({{labelName, labelValue}});
    }

    prometheus::Family<prometheus::Counter> *MetricService::GetCounter(const std::string &name) const {
        if (const auto it = _counterMap.find(name); it != _counterMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    void MetricService::ClearCounter(const std::string &name) const {
        boost::mutex::scoped_lock lock(_counterMutex);

        if (CounterExists(name)) {
            const auto counter = GetCounter(name);
            counter->Add({{}}).Reset();
            log_trace << "Counter cleared, name: " << name;
            return;
        }
        log_error << "Counter not found, name: " << name;
    }

    void MetricService::ClearCounter(const std::string &name, const std::string &labelName, const std::string &labelValue) const {
        boost::mutex::scoped_lock lock(_counterMutex);
        if (CounterExists(name, labelName, labelValue)) {
            const auto counter = GetCounter(name);
            counter->Add({{labelName, labelValue}}).Reset();
            log_trace << "Counter cleared, name: " << name << " labelName: " << labelName << " labelValue: " << labelValue;
            return;
        }
        log_error << "Counter not found, name: " << name << " labelName: " << labelName << " labelValue: " << labelValue;
    }

    void MetricService::IncrementCounter(const std::string &name, const int value) {
        if (!CounterExists(name)) {
            AddCounter(name);
        }
        const auto counter = GetCounter(name);
        counter->Add({}).Increment(value);
        _database.IncCounter(name, value);
        log_trace << "Counter incremented, name: " << name;
    }

    void MetricService::IncrementCounter(const std::string &name, const std::string &labelName, const std::string &labelValue, const int value) {
        if (!CounterExists(name, labelName, labelValue)) {
            AddCounter(name, labelName, labelValue);
        }
        const auto counter = GetCounter(name);
        counter->Add({{labelName, labelValue}}).Increment(value);
        _database.IncCounter(name, value, labelName, labelValue);
        log_trace << "Counter incremented, name: " << name << " labelName: " << labelName << " labelValue: " << labelValue;
    }

    void MetricService::AddGauge(const std::string &name) {
        boost::mutex::scoped_lock lock(_gaugeMutex);
        DoAddGauge(name);
    }

    void MetricService::AddGauge(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        boost::mutex::scoped_lock lock(_gaugeMutex);
        try {
            if (!GaugeExists(name)) {
                DoAddGauge(name);
            }
            if (!GaugeExists(name, labelName, labelValue)) {
                GetGauge(name)->Add({{labelName, labelValue}});
            }
        } catch (std::exception &e) {
            log_error << e.what();
        }
    }

    void MetricService::DoAddGauge(const std::string &name) {
        try {
            auto &gauge = prometheus::BuildGauge()
                                  .Name(name)
                                  .Help(name)
                                  .Register(*_registry);
            _gaugeMap[name] = &gauge;
            log_trace << "Gauge added, name: " << name;
        } catch (std::exception &e) {
            log_error << e.what();
        }
    }
    prometheus::Family<prometheus::Gauge> *MetricService::GetGauge(const std::string &name) {
        if (const auto it = _gaugeMap.find(name); it != _gaugeMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    void MetricService::SetGauge(const std::string &name, const double value) {
        if (!GaugeExists(name)) {
            AddGauge(name);
        }
        const auto gauge = GetGauge(name);
        gauge->Add({}).Set(value);
        _database.SetGauge(name, value);
        log_trace << "Gauge value set, name: " << name;
    }

    void MetricService::SetGauge(const std::string &name, const std::string &labelName, const std::string &labelValue, const double value) {
        if (!GaugeExists(name, labelName, labelValue)) {
            AddGauge(name, labelName, labelValue);
        }
        _gaugeMap[name]->Add({{labelName, labelValue}}).Set(value);
        _database.SetGauge(name, value, labelName, labelValue);
        log_trace << "Gauge value set, name: " << name;
    }

    bool MetricService::GaugeExists(const std::string &name) const {
        return _gaugeMap.contains(name);
    }

    bool MetricService::GaugeExists(const std::string &name, const std::string &labelName, const std::string &labelValue) {
        return _gaugeMap.contains(name) && _gaugeMap[name]->Has({{labelName, labelValue}});
    }

}// namespace AwsMock::Monitoring