//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/SQSMemoryDb.h>

#include "awsmock/core/PagingUtils.h"

namespace AwsMock::Database {

    boost::mutex SQSMemoryDb::_sqsQueueMutex;
    boost::mutex SQSMemoryDb::_sqsMessageMutex;

    bool SQSMemoryDb::QueueExists(const std::string &region, const std::string &name) {

        return std::ranges::find_if(_queues,
                                    [region, name](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                                        return queue.second.region == region && queue.second.name == name;
                                    }) != _queues.end();
    }

    bool SQSMemoryDb::QueueUrlExists(const std::string &region, const std::string &queueUrl) {

        return std::ranges::find_if(_queues,
                                    [region, queueUrl](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                                        return queue.second.region == region && queue.second.queueUrl == queueUrl;
                                    }) != _queues.end();
    }

    bool SQSMemoryDb::QueueArnExists(const std::string &queueArn) {

        return std::ranges::find_if(_queues, [queueArn](const std::pair<std::string, Entity::SQS::Queue> &queue) {
            return queue.second.queueArn == queueArn;
        }) != _queues.end();
    }

    Entity::SQS::Queue SQSMemoryDb::CreateQueue(const Entity::SQS::Queue &queue) {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _queues[oid] = queue;
        log_trace << "Queue created, oid: " << oid;
        return GetQueueById(oid);
    }

    Entity::SQS::Queue SQSMemoryDb::GetQueueById(const std::string &oid) {

        const auto it = std::ranges::find_if(_queues, [oid](const std::pair<std::string, Entity::SQS::Queue> &queue) {
            return queue.first == oid;
        });

        if (it != _queues.end()) {
            it->second.oid = oid;
            return it->second;
        }
        return {};
    }

    Entity::SQS::Queue SQSMemoryDb::GetQueueByArn(const std::string &queueArn) {

        Entity::SQS::Queue result;

        const auto it =
                std::ranges::find_if(_queues, [queueArn](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                    return queue.second.queueArn == queueArn;
                });

        if (it != _queues.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::SQS::Queue SQSMemoryDb::GetQueueByUrl(const std::string &queueUrl) {

        Entity::SQS::Queue result;
        const auto it =
                std::ranges::find_if(_queues, [queueUrl](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                    return queue.second.queueUrl == queueUrl;
                });

        if (it != _queues.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::SQS::Queue SQSMemoryDb::GetQueueByName(const std::string &region, const std::string &queueName) {

        Entity::SQS::Queue result;
        const auto it = std::ranges::find_if(_queues,
                                             [region, queueName](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                                                 return queue.second.region == region && queue.second.name == queueName;
                                             });

        if (it != _queues.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::SQS::Queue SQSMemoryDb::GetQueueByDlq(const std::string &dlqQueueArn) {

        Entity::SQS::Queue result;
        const auto it =
                std::ranges::find_if(_queues, [dlqQueueArn](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                    return queue.second.attributes.redrivePolicy.deadLetterTargetArn == dlqQueueArn;
                });

        if (it != _queues.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::SQS::QueueList SQSMemoryDb::ListQueues(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) {

        const auto q = Core::from(QueuesToVector());
        if (!region.empty()) {
            q.where([region](const Entity::SQS::Queue &item) { return item.region == region; });
        }
        if (!prefix.empty()) {
            q.where([prefix](const Entity::SQS::Queue &item) { return Core::StringUtils::StartsWith(item.name, prefix); });
        }
        log_trace << "Got queue list, size: " << q.to_vector().size();
        if (!sortColumns.empty()) {
            std::ranges::sort(q.to_vector(), [sortColumns](const Entity::SQS::Queue &a, const Entity::SQS::Queue &b) {
                for (const auto &[column, sortDirection]: sortColumns) {
                    if (column == "name") {
                        return sortDirection == 1 ? a.name < b.name : b.name < a.name;
                    }
                    if (column == "size") {
                        return sortDirection == 1 ? a.size < b.size : b.size < a.size;
                    }
                    if (column == "messages") {
                        return sortDirection == 1 ? a.attributes.approximateNumberOfMessages < b.attributes.approximateNumberOfMessages : b.attributes.approximateNumberOfMessages < a.attributes.approximateNumberOfMessages;
                    }
                }
                return false;
            });
        }
        if (pageSize > 0) {
            return q.to_vector();
        }
        return Core::PageVector(q.to_vector(), pageSize, pageIndex);
    }

    Entity::SQS::QueueList SQSMemoryDb::ExportQueues(const std::vector<SortColumn> &sortColumns) {

        Entity::SQS::QueueList queueList = QueuesToVector();

        log_trace << "Got queue list, size: " << queueList.size();
        std::ranges::sort(queueList, [sortColumns](const Entity::SQS::Queue &a, const Entity::SQS::Queue &b) {
            for (const auto &[column, sortDirection]: sortColumns) {
                if (column == "name") {
                    return sortDirection == 1 ? a.name < b.name : b.name < a.name;
                }
                if (column == "size") {
                    return sortDirection == 1 ? a.size < b.size : b.size < a.size;
                }
                if (column == "messages") {
                    return sortDirection == 1 ? a.attributes.approximateNumberOfMessages < b.attributes.approximateNumberOfMessages : b.attributes.approximateNumberOfMessages < a.attributes.approximateNumberOfMessages;
                }
            }
            return false;
        });
        return queueList;
    }

    void SQSMemoryDb::ImportQueue(Entity::SQS::Queue &queue) {

        UpdateQueue(queue);
    }

    long SQSMemoryDb::PurgeQueue(const std::string &queueArn) {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);

        const auto count = std::erase_if(_messages, [queueArn](const auto &item) {
            auto const &[key, value] = item;
            return value.queueArn == queueArn;
        });
        log_debug << "Purged queue, queueArn: " << queueArn << " count: " << count;
        return static_cast<long>(count);
    }

    Entity::SQS::Queue SQSMemoryDb::UpdateQueue(Entity::SQS::Queue &queue) {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);

        queue.modified = system_clock::now();

        std::string region = queue.region;
        std::string name = queue.name;
        const auto it = std::ranges::find_if(_queues,
                                             [region, name](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                                                 return queue.second.region == region && queue.second.name == name;
                                             });
        _queues[it->first] = queue;
        return _queues[it->first];
    }

    void SQSMemoryDb::UpdateQueueInvisibleNumber(const std::string &queueArn, long messageNumber) {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);

        const auto it = std::ranges::find_if(_queues,
                                             [queueArn](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                                                 return queue.second.queueArn == queueArn;
                                             });
        if (it != _queues.end()) {
            it->second.modified = system_clock::now();
            it->second.attributes.approximateNumberOfMessages -= messageNumber;
            it->second.attributes.approximateNumberOfMessagesNotVisible += messageNumber;
        }
    }

    void SQSMemoryDb::UpdateQueueCounter(const std::string &queueArn, const long keys, const long size, long initial, const long invisible, const long delayed) {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);

        const auto it = std::ranges::find_if(_queues,
                                             [queueArn](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                                                 return queue.second.queueArn == queueArn;
                                             });
        if (it != _queues.end()) {
            it->second.size = size;
            it->second.attributes.approximateNumberOfMessages = keys;
            it->second.attributes.approximateNumberOfMessagesNotVisible = invisible;
            it->second.attributes.approximateNumberOfMessagesDelayed = delayed;
        }
    }

    long SQSMemoryDb::CountQueues(const std::string &prefix, const std::string &region) const {

        long count = 0;

        if (region.empty() && prefix.empty()) {
            count = static_cast<long>(_queues.size());
        } else {

            for (const auto &val: _queues | std::views::values) {
                if (!region.empty() && val.region == region || (!prefix.empty() && Core::StringUtils::StartsWith(val.name, prefix))) {
                    count++;
                }
            }
        }
        log_trace << "Count queues, result: " << count;
        return count;
    }

    long SQSMemoryDb::GetQueueSize(const std::string &queueArn) const {

        long sum = 0;
        std::for_each(_messages.rbegin(), _messages.rend(), [&](const std::pair<std::string, Entity::SQS::Message> &m) {
            if (m.second.queueArn == queueArn) {
                sum += m.second.size;
            }
        });
        log_trace << "Sum size, arn: " << queueArn << " sum: " << sum;
        return sum;
    }

    long SQSMemoryDb::DeleteQueue(const Entity::SQS::Queue &queue) {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);

        std::string region = queue.region;
        std::string queueUrl = queue.queueUrl;
        const auto count = std::erase_if(_queues, [region, queueUrl](const auto &item) {
            auto const &[key, value] = item;
            return value.region == region && value.queueUrl == queueUrl;
        });
        log_debug << "Queues deleted, count: " << count;
        return count;
    }

    long SQSMemoryDb::DeleteAllQueues() {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);

        const long deleted = _queues.size();
        log_debug << "All queues deleted, count: " << _queues.size();
        _queues.clear();
        return deleted;
    }

    Entity::SQS::Message SQSMemoryDb::CreateMessage(const Entity::SQS::Message &message) {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _messages[oid] = message;
        log_trace << "Message created, oid: " << oid;
        return GetMessageById(oid);
    }

    bool SQSMemoryDb::MessageExists(const std::string &receiptHandle) const {

        return std::ranges::find_if(_messages,
                                    [receiptHandle](const std::pair<std::string, Entity::SQS::Message> &message) {
                                        return message.second.receiptHandle == receiptHandle;
                                    }) != _messages.end();
    }

    bool SQSMemoryDb::MessageExistsByMessageId(const std::string &messageId) const {

        return std::ranges::find_if(_messages,
                                    [messageId](const std::pair<std::string, Entity::SQS::Message> &message) {
                                        return message.second.messageId == messageId;
                                    }) != _messages.end();
    }

    Entity::SQS::Message SQSMemoryDb::GetMessageById(const std::string &oid) {

        const auto it =
                std::ranges::find_if(_messages, [oid](const std::pair<std::string, Entity::SQS::Message> &message) {
                    return message.first == oid;
                });

        if (it != _messages.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::SQS::Message SQSMemoryDb::GetMessageByReceiptHandle(const std::string &receiptHandle) {

        Entity::SQS::Message result = {};
        const auto it = std::ranges::find_if(_messages,
                                             [receiptHandle](const std::pair<std::string, Entity::SQS::Message> &message) {
                                                 return message.second.receiptHandle == receiptHandle;
                                             });

        if (it != _messages.end()) {
            it->second.oid = it->first;
            result = it->second;
        }
        return result;
    }

    Entity::SQS::Message SQSMemoryDb::GetMessageByMessageId(const std::string &messageId) {

        Entity::SQS::Message result = {};
        const auto it = std::ranges::find_if(_messages,
                                             [messageId](const std::pair<std::string, Entity::SQS::Message> &message) {
                                                 return message.second.messageId == messageId;
                                             });

        if (it != _messages.end()) {
            it->second.oid = it->first;
            result = it->second;
        }
        return result;
    }

    Entity::SQS::Message SQSMemoryDb::UpdateMessage(Entity::SQS::Message &message) {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        message.modified = system_clock::now();

        std::string oid = message.oid;
        const auto it =
                std::ranges::find_if(_messages, [oid](const std::pair<std::string, Entity::SQS::Message> &message) {
                    return message.second.oid == oid;
                });
        _messages[it->first] = message;
        return _messages[it->first];
    }

    Entity::SQS::MessageList SQSMemoryDb::ListMessages(const std::string &region) {

        Entity::SQS::MessageList messageList;
        if (region.empty()) {

            for (const auto &val: _messages | std::views::values) {
                messageList.emplace_back(val);
            }

        } else {

            for (const auto &val: _messages | std::views::values) {
                if (Core::StringUtils::Contains(val.queueArn, region)) {
                    messageList.emplace_back(val);
                }
            }
        }
        log_trace << "Got message list, size: " << messageList.size();
        return messageList;
    }

    void SQSMemoryDb::ReceiveMessages(const std::string &queueArn, const long visibility, const long maxResult, const std::string &dlQueueArn, long maxRetries, Entity::SQS::MessageList &messageList) {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        const auto reset = system_clock::now() + std::chrono::seconds{visibility};

        // Get the cursor
        for (auto [fst, snd]: _messages) {

            if (snd.queueArn == queueArn && snd.status == Entity::SQS::MessageStatus::INITIAL) {

                snd.retries++;
                if (!dlQueueArn.empty() && maxRetries > 0 && snd.retries >= maxRetries) {

                    snd.queueArn = dlQueueArn;
                    snd.queueName = Core::AwsUtils::ConvertSQSQueueArnToName(dlQueueArn);
                    snd.receiptHandle = "";
                    snd.status = Entity::SQS::MessageStatus::INITIAL;
                    log_debug << "Message send to DQL, id: " << snd.oid << " queueArn: " << dlQueueArn;

                } else {

                    snd.reset = reset;
                    snd.status = Entity::SQS::MessageStatus::INVISIBLE;
                    snd.receiptHandle = Core::AwsUtils::CreateSqsReceiptHandler();
                }
                messageList.push_back(snd);

                // Update store
                _messages[fst] = snd;

                // Check max resources
                if (messageList.size() >= maxResult) {
                    break;
                }
            }
        }

        log_trace << "Messages received, queueArn: " << queueArn + " count: " << messageList.size();
    }

    long SQSMemoryDb::ResetMessages(const std::string &queueArn, long visibility) {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        long count = 0;
        for (auto [fst, snd]: _messages) {

            if (snd.queueArn == queueArn && snd.status == Entity::SQS::MessageStatus::INVISIBLE && snd.reset < std::chrono::system_clock::now()) {

                // Reset status
                snd.status = Entity::SQS::MessageStatus::INITIAL;
                snd.receiptHandle = "";

                // Update store
                _messages[fst] = snd;
                count++;
            }
            log_trace << "Message reset, visibilityTimeout: " << visibility << " updated: " << count << " queueArn: " << queueArn;
        }
        return count;
    }

    long SQSMemoryDb::RelocateToDlqMessages(const std::string &queueArn, const Entity::SQS::RedrivePolicy &redrivePolicy) {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        long count = 0;
        std::string dlqQueueUrl = Core::AwsUtils::ConvertSQSQueueArnToUrl(redrivePolicy.deadLetterTargetArn);
        for (auto [fst, snd]: _messages) {

            if (snd.queueArn == queueArn && snd.status == Entity::SQS::MessageStatus::INITIAL && snd.retries > redrivePolicy.maxReceiveCount) {

                snd.retries = 0;
                snd.queueArn = redrivePolicy.deadLetterTargetArn;
                _messages[fst] = snd;

                count++;
            }
        }
        log_trace << "Message redrive, arn: " << redrivePolicy.deadLetterTargetArn << " updated: " << count;
        return count;
    }

    long SQSMemoryDb::ResetDelayedMessages(const std::string &queueArn) {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        long count = 0;
        for (auto &[fst, snd]: _messages) {

            if (snd.queueArn == queueArn && snd.status == Entity::SQS::MessageStatus::DELAYED && snd.reset < system_clock::now()) {

                snd.status = Entity::SQS::MessageStatus::INITIAL;
                _messages[fst] = snd;

                count++;
            }
        }
        log_trace << "Delayed message reset, updated: " << count << " queueArn: " << queueArn;
        return count;
    }

    long SQSMemoryDb::RedriveMessages(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue) {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        long count = 0;
        for (auto [fst, snd]: _messages) {

            if (snd.queueArn == dlqQueue.queueArn) {

                snd.retries = 0;
                snd.queueArn = originalQueue.queueArn;
                snd.queueName = originalQueue.name;
                snd.status = Entity::SQS::MessageStatus::INITIAL;
                _messages[fst] = snd;

                count++;
            }
        }
        log_trace << "Message redrive, arn: " << dlqQueue.queueArn << " updated: " << count;
        return count;
    }

    void SQSMemoryDb::RedriveMessage(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue, const std::string &messageId) {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        for (auto [fst, snd]: _messages) {

            if (snd.queueArn == dlqQueue.queueArn && snd.messageId == messageId) {

                snd.retries = 0;
                snd.queueArn = originalQueue.queueArn;
                snd.queueName = originalQueue.name;
                snd.status = Entity::SQS::MessageStatus::INITIAL;
                _messages[fst] = snd;
            }
        }
        log_trace << "Message redrive, arn: " << dlqQueue.queueArn << ", messageId: " << messageId;
    }

    long SQSMemoryDb::MessageRetention(const std::string &queueArn, const long retentionPeriod) {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        long count = 0;
        for (auto &val: _messages | std::views::values) {

            if (val.queueArn == queueArn && val.status == Entity::SQS::MessageStatus::DELAYED && val.reset < system_clock::now()) {

                DeleteMessage(val);
                count++;
            }
        }
        log_trace << "Message retention reset, deleted: " << count << " queueArn: " << queueArn;
        return count;
    }

    long SQSMemoryDb::CountMessages(const std::string &queueArn) {
        if (queueArn.empty()) {
            return static_cast<long>(_messages.size());
        }
        return static_cast<long>(std::ranges::count_if(_messages, [queueArn](const auto &pair) {
            return pair.second.queueArn == queueArn;
        }));
    }

    long SQSMemoryDb::CountMessagesByStatus(const std::string &queueArn, const Entity::SQS::MessageStatus &status) {
        return static_cast<long>(std::ranges::count_if(_messages, [queueArn, status](const auto &pair) {
            return pair.second.queueArn == queueArn && pair.second.status == status;
        }));
    }

    Entity::SQS::MessageWaitTime SQSMemoryDb::GetAverageMessageWaitingTime() {

        Entity::SQS::MessageWaitTime waitTime{};
        for (const Entity::SQS::QueueList queueList = ListQueues(); const auto &queue: queueList) {

            // Extract map values
            std::vector<Entity::SQS::Message> filtered;
            std::ranges::transform(_messages,
                                   std::back_inserter(filtered),
                                   [queue](auto &kv) {
                                       if (kv.second.queueArn == queue.queueArn) {
                                           return kv.second;
                                       }
                                       return Entity::SQS::Message();
                                   });

            if (!filtered.empty()) {

                // Sort by created timestamp
                std::ranges::sort(filtered, [](auto x, auto y) { return x.created > y.created; });

                const double min = std::chrono::duration<double, std::milli>(system_clock::now() - filtered.front().created).count();

                if (const double max = std::chrono::duration<double, std::milli>(system_clock::now() - filtered.back().created).count(); max + min > 5) {
                    waitTime.waitTime[queue.name] = (max + min) / 2.0;
                } else {
                    waitTime.waitTime[queue.name] = 0.0;
                }
            } else {
                waitTime.waitTime[queue.name] = 0.0;
            }
        }
        return waitTime;
    }

    long SQSMemoryDb::DeleteMessages(const std::string &queueArn) {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        const auto count = std::erase_if(_messages, [queueArn](const auto &item) {
            auto const &[key, value] = item;
            return value.queueArn == queueArn;
        });

        log_debug << "Messages deleted, queueArn: " << queueArn << " count: " << count;
        return static_cast<long>(count);
    }

    long SQSMemoryDb::DeleteMessage(const Entity::SQS::Message &message) {
        return DeleteMessage(message.receiptHandle);
    }

    long SQSMemoryDb::DeleteMessage(const std::string &receiptHandle) {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        const auto count = std::erase_if(_messages, [receiptHandle](const std::pair<std::string, Entity::SQS::Message> &item) {
            return item.second.receiptHandle == receiptHandle;
        });
        log_debug << "Messages deleted, receiptHandle: " << receiptHandle << " count: " << count;
        return static_cast<long>(count);
    }

    long SQSMemoryDb::DeleteAllMessages() {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        const long count = static_cast<long>(_messages.size());
        _messages.clear();
        log_debug << "All messages deleted, count: " << count;
        return count;
    }

    void SQSMemoryDb::AdjustMessageCounters() {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        for (const auto &queue: _queues | std::views::values) {
            _queues[queue.oid].attributes.approximateNumberOfMessages = CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::INITIAL);
            _queues[queue.oid].attributes.approximateNumberOfMessagesNotVisible = CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::INVISIBLE);
            _queues[queue.oid].attributes.approximateNumberOfMessagesDelayed = CountMessagesByStatus(queue.queueArn, Entity::SQS::MessageStatus::DELAYED);
        }
        log_debug << "All message counters updated, count: " << _queues.size();
    }

    inline Entity::SQS::QueueList SQSMemoryDb::QueuesToVector() {
        Entity::SQS::QueueList queueList;
        std::ranges::transform(_queues, std::back_inserter(queueList), [](auto const &pair) { return pair.second; });
        return queueList;
    }
} // namespace AwsMock::Database
