//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/sns/SNSServer.h>

namespace AwsMock::Service {

    SNSServer::SNSServer(Core::Scheduler &scheduler) : AbstractServer("sns"), _monitoringCollector(Core::MonitoringCollector::instance()), _scheduler(scheduler) {

        // Configuration
        _deletePeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.sns.delete-period");
        _counterPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.sns.counter-period");
        _monitoringPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.sns.monitoring-period");
        _backupActive = Core::Configuration::instance().GetValue<bool>("awsmock.modules.sns.backup.active");
        _backupCron = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.sns.backup.cron");

        // Start SNS monitoring update counters
        _scheduler.AddTask("sns-monitoring", [this] { UpdateCounter(); }, _counterPeriod);

        // Start the delete old messages task
        _scheduler.AddTask("sns-delete-messages", [this] { DeleteOldMessages(); }, _deletePeriod);

        // Start backup
        if (_backupActive) {
            _scheduler.AddTask("sns-backup", [] { BackupSns(); }, _backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&SNSServer::Shutdown, this));

        log_debug << "SNS server initialized, workerPeriod: " << _deletePeriod << " monitoringPeriod: " << _monitoringPeriod;
    }

    void SNSServer::DeleteOldMessages() const {
        const int messageTimeout = Core::Configuration::instance().GetValue<int>("awsmock.modules.sns.timeout");
        _snsDatabase.DeleteOldMessages(messageTimeout);
    }

    void SNSServer::UpdateCounter() const {

        log_trace << "SNS counter update starting";

        // Reload the counters first
        _snsDatabase.AdjustMessageCounters();

        long totalMessages = 0;
        long totalSize = 0;
        const Database::Entity::SNS::TopicList topicList = _snsDatabase.ListTopics();
        for (auto &topic: topicList) {

            _monitoringCollector.SetGauge(SNS_MESSAGE_BY_TOPIC_COUNT, "topic", topic.topicName, static_cast<double>(topic.messages));
            _monitoringCollector.SetGauge(SNS_TOPIC_SIZE, "topic", topic.topicName, static_cast<double>(topic.size));
            totalMessages += topic.messages;
            totalSize += topic.size;
        }
        _monitoringCollector.SetGauge(SNS_TOPIC_COUNT, {}, {}, static_cast<double>(topicList.size()));
        _monitoringCollector.SetGauge(SNS_MESSAGE_COUNT, {}, {}, static_cast<double>(totalMessages));
        log_debug << "SNS monitoring finished";
    }

    void SNSServer::BackupSns() {
        ModuleService::BackupModule("sns", true);
    }

    void SNSServer::Shutdown() {
        log_info << "SNS manager server shutting down";
        _scheduler.Shutdown("sns-monitoring");
        _scheduler.Shutdown("sns-delete-messages");
        _scheduler.Shutdown("sns-backup");
    }
}// namespace AwsMock::Service
