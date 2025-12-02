//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/sqs/SQSServer.h>

namespace AwsMock::Service {
    SQSServer::SQSServer(Core::Scheduler &scheduler) : AbstractServer("sqs"), _monitoringCollector(Core::MonitoringCollector::instance()) {

        _monitoringPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.sqs.monitoring-period");
        _resetPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.sqs.reset-period");
        _counterPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.sqs.counter-period");
        _backupActive = Core::Configuration::instance().GetValue<bool>("awsmock.modules.sqs.backup.active");
        _backupCron = Core::Configuration::instance().GetValue<std::string>("awsmock.modules.sqs.backup.cron");

        // Check module active
        if (!IsActive("sqs")) {
            log_info << "SQS module inactive";
            return;
        }
        log_info << "SQS server starting";

        // Start SQS monitoring update counters
        scheduler.AddTask("sqs-monitoring", [this] { this->UpdateCounter(); }, _counterPeriod);
        scheduler.AddTask("sqs-monitoring-wait-time", [this] { this->CollectWaitingTimeStatistics(); }, _monitoringPeriod, _monitoringPeriod);

        // Start reset messages task
        scheduler.AddTask("sqs-reset-messages", [this] { this->ResetMessages(); }, _resetPeriod, _resetPeriod);
        scheduler.AddTask("sqs-setdlq", [this] { this->SetDlq(); }, -1);
        scheduler.AddTask("sqs-seturl", [this] { this->SetUrl(); }, -1);

        // Start backup
        if (_backupActive) {
            scheduler.AddTask("sqs-backup", [] { BackupSqs(); }, _backupCron);
        }

        // Set running
        SetRunning();

        log_debug << "SQS server initialized";
    }

    void SQSServer::ResetMessages() const {
        Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();
        log_trace << "SQS reset messages starting, count: " << queueList.size();

        if (queueList.empty()) {
            return;
        }

        // Loop over queues and do some maintenance work
        for (auto &queue: queueList) {

            if (const long messageCount = _sqsDatabase.CountMessages(queue.queueArn); messageCount > 0) {

                // Check the retention period
                if (queue.attributes.messageRetentionPeriod > 0) {
                    queue.attributes.approximateNumberOfMessages -= _sqsDatabase.MessageRetention(queue.queueUrl, queue.attributes.messageRetentionPeriod);
                }

                // Check visibility timeout
                if (queue.attributes.visibilityTimeout > 0) {
                    queue.attributes.approximateNumberOfMessagesNotVisible -= _sqsDatabase.ResetMessages(queue.queueArn, queue.attributes.visibilityTimeout);
                }

                // Check delays
                if (queue.attributes.delaySeconds > 0) {
                    queue.attributes.approximateNumberOfMessagesDelayed -= _sqsDatabase.ResetDelayedMessages(queue.queueUrl, queue.attributes.delaySeconds);
                }

                // Save results
                queue = _sqsDatabase.UpdateQueue(queue);
                log_trace << "Queue updated, queueName" << queue.name;
            }
        }
        log_trace << "SQS reset messages finished, count: " << queueList.size();
    }

    void SQSServer::SetDlq() const {
        const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();
        log_trace << "SQS set DLQ task starting, count: " << queueList.size();

        if (queueList.empty()) {
            return;
        }

        // Loop over queues and do some maintenance work
        for (auto &queue: queueList) {

            if (!queue.attributes.redrivePolicy.deadLetterTargetArn.empty()) {

                Database::Entity::SQS::Queue dlq = _sqsDatabase.GetQueueByArn(queue.attributes.redrivePolicy.deadLetterTargetArn);
                dlq.isDlq = true;
                dlq.mainQueue = queue.queueArn;

                // Save results
                dlq = _sqsDatabase.UpdateQueue(dlq);
                log_trace << "DLQ updated, queueName" << dlq.name;
            }
        }
        log_trace << "SQS DQL finished, count: " << queueList.size();
    }

    void SQSServer::SetUrl() const {

        Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();
        log_trace << "SQS set URL task starting, count: " << queueList.size();

        if (queueList.empty()) {
            return;
        }

        // Loop over queues and do some maintenance work
        for (auto &queue: queueList) {

            queue.queueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(queue.queueArn);
            queue = _sqsDatabase.UpdateQueue(queue);
            log_trace << "SQS queue updated, queueName" << queue.name;
        }
        log_trace << "SQS URL task finished, count: " << queueList.size();
    }

    void SQSServer::UpdateCounter() const {

        log_trace << "SQS counter update starting";

        // Reload the counters first
        _sqsDatabase.AdjustMessageCounters();

        long totalMessages = 0;
        long totalSize = 0;
        const Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();
        for (auto &queue: queueList) {
            _monitoringCollector.SetGauge(SQS_MESSAGE_BY_QUEUE_COUNT_TOTAL, "queue", queue.name, static_cast<double>(queue.attributes.approximateNumberOfMessages));
            _monitoringCollector.SetGauge(SQS_QUEUE_SIZE, "queue", queue.name, static_cast<double>(queue.size));
            totalMessages += queue.attributes.approximateNumberOfMessages;
            totalSize += queue.size;
        }
        _monitoringCollector.SetGauge(SQS_QUEUE_COUNT, {}, {}, static_cast<double>(queueList.size()));
        _monitoringCollector.SetGauge(SQS_MESSAGE_COUNT, {}, {}, static_cast<double>(totalMessages));
        log_debug << "SQS monitoring finished";
    }

    void SQSServer::CollectWaitingTimeStatistics() const {
        log_trace << "SQS message wait time starting";

        auto [waitTime] = _sqsDatabase.GetAverageMessageWaitingTime();
        log_trace << "SQS worker starting, count: " << waitTime.size();

        if (!waitTime.empty()) {
            for (auto &[fst, snd]: waitTime) {
                _monitoringCollector.SetGauge(SQS_MESSAGE_WAIT_TIME, "queue", fst, snd);
            }
        }
        log_trace << "SQS wait time update finished";
    }

    void SQSServer::BackupSqs() {
        ModuleService::BackupModule("sqs", true);
    }

}// namespace AwsMock::Service
