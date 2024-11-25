//
// Created by vogje01 on 03/06/2023.
//

#include "awsmock/service/sns/SNSServer.h"

namespace AwsMock::Service {
    SNSServer::SNSServer(Core::PeriodicScheduler &scheduler) : AbstractServer("sns") {
        // HTTP manager configuration
        Core::YamlConfiguration &configuration = Core::YamlConfiguration::instance();
        _deletePeriod = configuration.GetValueInt("awsmock.modules.sns.delete.period");
        _counterPeriod = configuration.GetValueInt("awsmock.modules.sns.counter.period");
        _monitoringPeriod = configuration.GetValueInt("awsmock.modules.sns.monitoring.period");

        // Check module active
        if (!IsActive("sns")) {
            log_info << "SNS module inactive";
            return;
        }
        log_info << "SNS server starting";

        // Start SNS monitoring update counters
        scheduler.AddTask("monitoring-sns-counters", [this] { UpdateCounter(); }, _monitoringPeriod);

        // Start delete old message task
        scheduler.AddTask("sns-delete-messages", [this] { DeleteOldMessages(); }, _deletePeriod);
        scheduler.AddTask("sns-synchronize-counters", [this] { SychronizeCounters(); }, _counterPeriod);

        // Set running
        SetRunning();

        log_debug << "SNS server initialized, workerPeriod: " << _deletePeriod << " monitoringPeriod: " <<
 _monitoringPeriod;
    }

    void SNSServer::DeleteOldMessages() const {
        const int messageTimeout = Core::YamlConfiguration::instance().GetValueInt("awsmock.modules.sns.timeout");
        _snsDatabase.DeleteOldMessages(messageTimeout);
    }

    void SNSServer::SychronizeCounters() const {
        for (auto &topic : _snsDatabase.ListTopics()) {
            topic.topicAttribute.availableMessages = _snsDatabase.CountMessages(topic.topicArn);
            _snsDatabase.UpdateTopic(topic);
        }
    }

    void SNSServer::UpdateCounter() const {
        log_trace << "SNS Monitoring starting";

        // Get total counts
        const long topics = _snsDatabase.CountTopics();
        const long messages = _snsDatabase.CountMessages();
        _metricService.SetGauge(SNS_TOPIC_COUNT, static_cast<double>(topics));
        _metricService.SetGauge(SNS_MESSAGE_COUNT, static_cast<double>(messages));

        // Count resources per topic
        for (const auto &topic : _snsDatabase.ListTopics()) {
            std::string labelValue = Poco::replace(topic.topicName, "-", "_");
            _metricService.SetGauge(SNS_MESSAGE_BY_TOPIC_COUNT,
                                    "topic",
                                    labelValue,
                                    static_cast<double>(topic.topicAttribute.availableMessages));
        }
        log_trace << "SNS monitoring finished";
    }
} // namespace AwsMock::Service
