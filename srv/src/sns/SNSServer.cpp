//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/sns/SNSServer.h>

namespace Awsmock::Service {

    SNSServer::SNSServer(Core::Scheduler &scheduler) : AbstractServer("sns"), _scheduler(scheduler) {

        // Configuration
        _deletePeriod = Core::Configuration::instance().get<int>("awsmock.modules.sns.delete-period");
        _counterPeriod = Core::Configuration::instance().get<int>("awsmock.modules.sns.counter-period");
        _monitoringPeriod = Core::Configuration::instance().get<int>("awsmock.modules.sns.monitoring-period");
        _backupActive = Core::Configuration::instance().get<bool>("awsmock.modules.sns.backup.active");
        _backupCron = Core::Configuration::instance().get<std::string>("awsmock.modules.sns.backup.cron");

        // Start SNS monitoring update counters
        _scheduler.AddTask("sns-monitoring", [this] { UpdateCounter(); }, _counterPeriod);

        // Start the delete old messages task
        _scheduler.AddTask("sns-delete-messages", [this] { DeleteOldMessages(); }, _deletePeriod);

        // Start backup
        if (_backupActive) {
            _scheduler.AddTask("sns-backup", [] { BackupSns(); }, _backupCron);
        }

        // Connect stop signal
        Core::EventBus::instance().sigShutdown.connect(boost::signals2::signal<void()>::slot_type(&SNSServer::shutdown, this));

        log_debug << "SNS server initialized, workerPeriod: " << _deletePeriod << " monitoringPeriod: " << _monitoringPeriod;
    }

    void SNSServer::DeleteOldMessages() const {
        const int messageTimeout = Core::Configuration::instance().get<int>("awsmock.modules.sns.timeout");
        _snsDatabase->deleteOldMessages(messageTimeout);
        log_debug << "SNS delete old messages finished, messageTimeout: " << messageTimeout;
    }

    void SNSServer::UpdateCounter() const {

        log_trace << "SNS counter update starting";

        // Reload the counters first
        _snsDatabase->adjustMessageCounters();

        long totalMessages = 0;
        long totalSize = 0;
        const Database::Entity::SNS::TopicList topicList = _snsDatabase->listTopics({});
        for (auto &topic: topicList) {

            Core::EventBus::instance().sigMetricGauge(SNS_MESSAGE_BY_TOPIC_COUNT, "topic", topic.topicName, topic.messages);
            Core::EventBus::instance().sigMetricGauge(SNS_TOPIC_SIZE, "topic", topic.topicName, topic.size);
            totalMessages += topic.messages;
            totalSize += topic.size;
        }
        Core::EventBus::instance().sigMetricGauge(SNS_TOPIC_COUNT, {}, {}, static_cast<double>(topicList.size()));
        Core::EventBus::instance().sigMetricGauge(SNS_MESSAGE_COUNT, {}, {}, totalMessages);
        log_debug << "SNS monitoring finished";
    }

    void SNSServer::BackupSns() {
        ModuleService{}.BackupModule("sns", Dto::Module::ExportType::INFRA_STRUCTURE);
    }

    void SNSServer::shutdown() {
        log_info << "SNS manager server shutting down";
        _scheduler.Shutdown("sns-monitoring");
        _scheduler.Shutdown("sns-delete-messages");
        _scheduler.Shutdown("sns-backup");
    }
}// namespace Awsmock::Service
