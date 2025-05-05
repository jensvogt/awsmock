//
// Created by vogje01 on 03/06/2023.
//

#include <awsmock/service/sqs/SQSServer.h>

namespace AwsMock::Service {
    SQSServer::SQSServer(Core::PeriodicScheduler &scheduler) : AbstractServer("sqs") {

        _monitoringPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.sqs.monitoring.period");
        _resetPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.sqs.reset.period");
        _counterPeriod = Core::Configuration::instance().GetValue<int>("awsmock.modules.sqs.counter.period");

        // Check module active
        if (!IsActive("sqs")) {
            log_info << "SQS module inactive";
            return;
        }
        log_info << "SQS server starting";

        // Initialize shared memory
        _segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, SHARED_MEMORY_SEGMENT_NAME);
        _sqsCounterMap = _segment.find<Database::SqsCounterMapType>(Database::SQS_COUNTER_MAP_NAME).first;

        // Start SQS monitoring update counters
        scheduler.AddTask("monitoring-sqs-counters", [this] { this->UpdateCounter(); }, _counterPeriod);
        scheduler.AddTask("monitoring-sqs-wait-time", [this] { this->CollectWaitingTimeStatistics(); }, _monitoringPeriod);

        // Start reset messages task
        scheduler.AddTask("sqs-reset-messages", [this] { this->ResetMessages(); }, _resetPeriod);
        //TODO: Check relocation scheme
        //scheduler.AddTask("sqs-relocate-messages", [this] { this->RelocateMessages(); }, _resetPeriod);
        scheduler.AddTask("sqs-setdlq", [this] { this->SetDlq(); }, _resetPeriod);
        //scheduler.AddTask("sqs-count-messages", [this] { this->AdjustCounters(); }, _counterPeriod);

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
        Database::Entity::SQS::QueueList queueList = _sqsDatabase.ListQueues();
        log_trace << "SQS relocate messages starting, count: " << queueList.size();

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

    void SQSServer::UpdateCounter() const {

        log_trace << "SQS counter update starting";

        if (_sqsCounterMap) {
            long totalMessages = 0;
            long totalSize = 0;
            for (auto const &[key, val]: *_sqsCounterMap) {

                std::string labelValue = key;
                Core::StringUtils::Replace(labelValue, "-", "_");

                _metricService.SetGauge(SQS_MESSAGE_BY_QUEUE_COUNT, "bucket", labelValue, static_cast<double>(val.messages));
                _metricService.SetGauge(SQS_QUEUE_SIZE, "bucket", labelValue, static_cast<double>(val.size));

                totalMessages += val.messages;
                totalSize += val.size;
                _sqsDatabase.UpdateQueueCounter(key, val.messages, val.size, val.initial, val.invisible, val.delayed);
            }
            _metricService.SetGauge(SQS_QUEUE_COUNT, static_cast<double>(_sqsCounterMap->size()));
            _metricService.SetGauge(SQS_MESSAGE_COUNT, static_cast<double>(totalMessages));
        }
        log_debug << "SQS monitoring finished, freeShmSize: " << _segment.get_free_memory();
    }

    void SQSServer::CollectWaitingTimeStatistics() const {
        log_trace << "SQS message wait time starting";

        auto [waitTime] = _sqsDatabase.GetAverageMessageWaitingTime();
        log_trace << "SQS worker starting, count: " << waitTime.size();

        if (!waitTime.empty()) {
            for (auto &[fst, snd]: waitTime) {
                _metricService.SetGauge(SQS_MESSAGE_WAIT_TIME, "queue", fst, snd);
            }
        }
        log_trace << "SQS wait time update finished";
    }
}// namespace AwsMock::Service
