//
// Created by vogje01 on 03/06/2023.
//

#include "awsmock/service/sns/SNSServer.h"

namespace AwsMock::Service {
    SNSServer::SNSServer(Core::PeriodicScheduler &scheduler) : AbstractServer("sns") {
        // HTTP manager configuration
        Core::Configuration &configuration = Core::Configuration::instance();
        _deletePeriod = configuration.GetValue<int>("awsmock.modules.sns.delete.period");
        _counterPeriod = configuration.GetValue<int>("awsmock.modules.sns.counter.period");
        _monitoringPeriod = configuration.GetValue<int>("awsmock.modules.sns.monitoring.period");

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
        scheduler.AddTask("sns-synchronize-counters", [this] { SynchronizeCounters(); }, _counterPeriod);

        // Set running
        SetRunning();

        log_debug << "SNS server initialized, workerPeriod: " << _deletePeriod << " monitoringPeriod: " << _monitoringPeriod;
    }

    void SNSServer::DeleteOldMessages() const {
        const int messageTimeout = Core::Configuration::instance().GetValue<int>("awsmock.modules.sns.timeout");
        _snsDatabase.DeleteOldMessages(messageTimeout);
    }

    void SNSServer::SynchronizeCounters() const {
        _snsDatabase.AdjustAllMessageCounters();
        for (auto &topic: _snsDatabase.ListTopics()) {
            topic.topicAttribute.availableMessages = _snsDatabase.CountMessages(topic.topicArn);
            topic.size = _snsDatabase.GetTopicSize(topic.topicArn);
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
        for (const auto &topic: _snsDatabase.ListTopics()) {

            std::string labelValue = topic.topicName;
            Core::StringUtils::Replace(labelValue, "-", "_");

            _metricService.SetGauge(SNS_MESSAGE_BY_TOPIC_COUNT, "topic", labelValue, static_cast<double>(topic.topicAttribute.availableMessages));
            _metricService.SetGauge(SNS_TOPIC_SIZE, "queue", labelValue, static_cast<double>(topic.size));
        }
        log_trace << "SNS monitoring finished";
    }
}// namespace AwsMock::Service
