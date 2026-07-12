//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/sqs/SQSMemoryRepository.h>

namespace Awsmock::Database {

    boost::mutex SQSMemoryRepository::_sqsQueueMutex;
    boost::mutex SQSMemoryRepository::_sqsMessageMutex;

    bool SQSMemoryRepository::queueExists(const std::string &region, const std::string &name) const {

        return std::ranges::find_if(_queues,
                                    [region, name](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                                        return queue.second.region == region && queue.second.name == name;
                                    }) != _queues.end();
    }

    bool SQSMemoryRepository::queueUrlExists(const std::string &region, const std::string &queueUrl) const {

        return std::ranges::find_if(_queues,
                                    [region, queueUrl](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                                        return queue.second.region == region && queue.second.url == queueUrl;
                                    }) != _queues.end();
    }

    bool SQSMemoryRepository::queueArnExists(const std::string &queueArn) const {

        return std::ranges::find_if(_queues, [queueArn](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                   return queue.second.arn == queueArn;
               }) != _queues.end();
    }

    Entity::SQS::Queue SQSMemoryRepository::createQueue(Entity::SQS::Queue &queue) const {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);

        queue.oid = Core::StringUtils::CreateRandomUuid();
        _queues[queue.oid] = queue;
        log_trace << "Queue created, oid: " << queue.oid;
        return _queues[queue.oid];
    }

    Entity::SQS::Queue SQSMemoryRepository::createOrUpdateQueue(Entity::SQS::Queue &queue) const {
        if (queueExists(queue.region, queue.name)) {
            return updateQueue(queue);
        }
        return createQueue(queue);
    }

    Entity::SQS::Queue SQSMemoryRepository::getQueueById(const std::string &oid) const {

        const auto it = std::ranges::find_if(_queues, [oid](const std::pair<std::string, Entity::SQS::Queue> &queue) {
            return queue.first == oid;
        });

        if (it != _queues.end()) {
            it->second.oid = oid;
            return it->second;
        }
        return {};
    }

    Entity::SQS::Queue SQSMemoryRepository::getQueueById(const bsoncxx::oid &oid) const {
        return getQueueById(oid.to_string());
    }

    Entity::SQS::Queue SQSMemoryRepository::getQueueByArn(const std::string &queueArn) const {

        const auto it = std::ranges::find_if(_queues, [queueArn](const std::pair<std::string, Entity::SQS::Queue> &queue) {
            return queue.second.arn == queueArn;
        });

        if (it != _queues.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::SQS::Queue SQSMemoryRepository::getQueueByUrl(const std::string &region, const std::string &queueUrl) const {

        Entity::SQS::Queue result;
        const auto it =
                std::ranges::find_if(_queues, [queueUrl, region](const std::pair<std::string, Entity::SQS::Queue> &queue) {
                    return queue.second.url == queueUrl && queue.second.region == region;
                });

        if (it != _queues.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        return {};
    }

    Entity::SQS::Queue SQSMemoryRepository::getQueueByName(const std::string &region, const std::string &queueName) const {

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

    Entity::SQS::Queue SQSMemoryRepository::getQueueByDlq(const std::string &dlqQueueArn) const {

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

    std::vector<Entity::SQS::Queue> SQSMemoryRepository::isDlq(const std::string &queueArn) const {

        auto q = Core::from(_queues | std::views::values | std::ranges::to<std::vector>());
        if (!queueArn.empty()) {
            q = q.where([queueArn](const Entity::SQS::Queue &item) { return item.attributes.redrivePolicy.deadLetterTargetArn == queueArn; });
        }
        return q.to_vector();
    }

    Entity::SQS::QueueList SQSMemoryRepository::listQueues(const std::string &prefix, const long pageSize, const long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) const {

        auto q = Core::from(_queues | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::SQS::Queue &item) { return item.region == region; });
        }
        if (!prefix.empty()) {
            q = q.where([prefix](const Entity::SQS::Queue &item) { return Core::StringUtils::StartsWith(item.name, prefix); });
        }
        log_trace << "Got queue list, size: " << q.to_vector().size();
        if (!sortColumns.empty()) {
            std::ranges::sort(q.to_vector(), [sortColumns](const Entity::SQS::Queue &a, const Entity::SQS::Queue &b) {
                for (const auto &sortColumn: sortColumns) {
                    if (sortColumn.column == "name") {
                        return sortColumn.sortDirection == 1 ? a.name < b.name : b.name < a.name;
                    }
                    if (sortColumn.column == "size") {
                        return sortColumn.sortDirection == 1 ? a.size < b.size : b.size < a.size;
                    }
                    if (sortColumn.column == "messages") {
                        return sortColumn.sortDirection == 1 ? a.attributes.approximateNumberOfMessages < b.attributes.approximateNumberOfMessages : b.attributes.approximateNumberOfMessages < a.attributes.approximateNumberOfMessages;
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

    Entity::SQS::QueueList SQSMemoryRepository::listQueues(const std::string &region) const {

        auto q = Core::from(_queues | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::SQS::Queue &item) { return item.region == region; });
        }
        log_trace << "Got queue list, size: " << q.to_vector().size();
        return q.to_vector();
    }

    Entity::SQS::QueueList SQSMemoryRepository::listQueues() const {
        return listQueues({});
    }

    std::vector<std::string> SQSMemoryRepository::listQueueUrls(const std::string &region, const std::string &prefix) const {

        auto q = Core::from(_queues | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::SQS::Queue &item) { return item.region == region; });
        }

        if (!region.empty()) {
            q = q.where([region](const Entity::SQS::Queue &item) { return item.region == region; });
        }

        log_trace << "Got queue URL list, size: " << q.count();
        return q.to_vector() | std::views::transform([](const Entity::SQS::Queue &queue) { return queue.url; }) | std::ranges::to<std::vector<std::string>>();
    }

    std::vector<std::string> SQSMemoryRepository::listQueueUrls() const {
        return listQueueUrls({}, {});
    }

    Entity::SQS::QueueList SQSMemoryRepository::exportQueues(const std::vector<SortColumn> &sortColumns) const {

        auto q = _queues | std::views::values | std::ranges::to<std::vector>();

        log_trace << "Got queue list, size: " << q.size();
        std::ranges::sort(q, [sortColumns](const Entity::SQS::Queue &a, const Entity::SQS::Queue &b) {
            for (const auto &sortColumn: sortColumns) {
                if (sortColumn.column == "name") {
                    return sortColumn.sortDirection == 1 ? a.name < b.name : b.name < a.name;
                }
                if (sortColumn.column == "size") {
                    return sortColumn.sortDirection == 1 ? a.size < b.size : b.size < a.size;
                }
                if (sortColumn.column == "messages") {
                    return sortColumn.sortDirection == 1 ? a.attributes.approximateNumberOfMessages < b.attributes.approximateNumberOfMessages : b.attributes.approximateNumberOfMessages < a.attributes.approximateNumberOfMessages;
                }
            }
            return false;
        });
        return q;
    }

    Entity::SQS::Queue SQSMemoryRepository::importQueue(Entity::SQS::Queue &queue) const {
        if (queueExists(queue.region, queue.name)) {
            return updateQueue(queue);
        }
        return createQueue(queue);
    }

    long SQSMemoryRepository::purgeQueue(const std::string &queueArn) const {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);

        const auto count = std::erase_if(_messages, [queueArn](const auto &item) {
            auto const &[key, value] = item;
            return value.queueArn == queueArn;
        });
        log_debug << "Purged queue, queueArn: " << queueArn << " count: " << count;
        return static_cast<long>(count);
    }

    Entity::SQS::Queue SQSMemoryRepository::updateQueue(Entity::SQS::Queue &queue) const {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);

        queue.modified = system_clock::now();

        const auto it = std::ranges::find_if(_queues,
                                             [queue](const std::pair<std::string, Entity::SQS::Queue> &q) {
                                                 return q.second.region == queue.region && q.second.name == queue.name;
                                             });
        if (it != _queues.end()) {
            _queues[it->first] = queue;
            return _queues[it->first];
        }
        return {};
    }

    long SQSMemoryRepository::countQueues(const std::string &region, const std::string &prefix) const {

        auto q = Core::from(_queues | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([prefix](const Entity::SQS::Queue &item) { return Core::StringUtils::StartsWith(item.name, prefix); });
        }
        if (!region.empty()) {
            q = q.where([prefix](const Entity::SQS::Queue &item) { return Core::StringUtils::StartsWith(item.name, prefix); });
        }
        return static_cast<long>(q.count());
    }

    long SQSMemoryRepository::deleteQueue(const Entity::SQS::Queue &queue) const {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);
        const auto count = std::erase_if(_queues, [queue](const auto &item) {
            return item.second.region == queue.region && item.second.url == queue.url;
        });
        log_debug << "Queues deleted, count: " << count;
        return static_cast<long>(count);
    }

    long SQSMemoryRepository::deleteAllQueues() const {
        boost::mutex::scoped_lock lock(_sqsQueueMutex);

        const long deleted = static_cast<long>(_queues.size());
        log_debug << "All queues deleted, count: " << _queues.size();
        _queues.clear();
        return deleted;
    }

    Entity::SQS::Message SQSMemoryRepository::createMessage(Entity::SQS::Message &message) const {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        message.oid = Core::StringUtils::CreateRandomUuid();
        _messages[message.oid] = message;
        log_trace << "Message created, oid: " << message.oid;
        return _messages[message.oid];
    }

    Entity::SQS::Message SQSMemoryRepository::createOrUpdateMessage(Entity::SQS::Message &message) const {
        if (messageExists(message.receiptHandle)) {
            return updateMessage(message);
        }
        return createMessage(message);
    }

    bool SQSMemoryRepository::messageExists(const std::string &receiptHandle) const {

        return std::ranges::find_if(_messages,
                                    [receiptHandle](const std::pair<std::string, Entity::SQS::Message> &message) {
                                        return message.second.receiptHandle == receiptHandle;
                                    }) != _messages.end();
    }

    bool SQSMemoryRepository::messageExistsByMessageId(const std::string &messageId) const {

        return std::ranges::find_if(_messages,
                                    [messageId](const std::pair<std::string, Entity::SQS::Message> &message) {
                                        return message.second.messageId == messageId;
                                    }) != _messages.end();
    }

    Entity::SQS::Message SQSMemoryRepository::getMessageById(const std::string &oid) const {

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

    Entity::SQS::Message SQSMemoryRepository::getMessageById(bsoncxx::oid oid) const {
        return getMessageById(oid.to_string());
    }

    Entity::SQS::Message SQSMemoryRepository::getMessageByReceiptHandle(const std::string &receiptHandle) const {

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

    Entity::SQS::Message SQSMemoryRepository::getMessageByMessageId(const std::string &messageId) const {

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

    Entity::SQS::Message SQSMemoryRepository::updateMessage(Entity::SQS::Message &message) const {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        message.modified = system_clock::now();

        std::string oid = message.oid;
        const auto it = std::ranges::find_if(_messages, [oid](const std::pair<std::string, Entity::SQS::Message> &message) {
            return message.second.oid == oid;
        });
        if (it != _messages.end()) {
            _messages[it->first] = message;
            return _messages[it->first];
        }
        return {};
    }

    Entity::SQS::MessageList SQSMemoryRepository::listMessages(const std::string &region) const {

        auto q = Core::from(_messages | std::views::values | std::ranges::to<std::vector>());
        if (!region.empty()) {
            q = q.where([region](const Entity::SQS::Message &item) { return Core::StringUtils::Contains(item.queueArn, region); });
        }
        return q.to_vector();
    }

    Entity::SQS::MessageList SQSMemoryRepository::listMessages(const std::string &queueArn, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        auto q = Core::from(_messages | std::views::values | std::ranges::to<std::vector>());
        if (!queueArn.empty()) {
            q = q.where([queueArn](const Entity::SQS::Message &item) { return Core::StringUtils::Contains(item.queueArn, queueArn); });
        }
        return q.to_vector();
    }

    void SQSMemoryRepository::receiveMessages(const std::string &queueArn, const long visibility, const long maxResult, const std::string &dlQueueArn, long maxRetries, Entity::SQS::MessageList &messageList) const {
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

    long SQSMemoryRepository::resetMessages(const std::string &queueArn, const long visibility) const {
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

    long SQSMemoryRepository::resetDelayedMessages(const std::string &queueArn, long delay) const {
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

    long SQSMemoryRepository::redriveMessages(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue) const {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        long count = 0;
        for (auto [fst, snd]: _messages) {

            if (snd.queueArn == dlqQueue.arn) {

                snd.retries = 0;
                snd.queueArn = originalQueue.arn;
                snd.queueName = originalQueue.name;
                snd.status = Entity::SQS::MessageStatus::INITIAL;
                _messages[fst] = snd;

                count++;
            }
        }
        log_trace << "Message redrive, arn: " << dlqQueue.arn << " updated: " << count;
        return count;
    }

    long SQSMemoryRepository::redriveMessage(const Entity::SQS::Queue &originalQueue, const Entity::SQS::Queue &dlqQueue, const std::string &messageId) const {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        long count{};
        for (auto [fst, snd]: _messages) {

            if (snd.queueArn == dlqQueue.arn && snd.messageId == messageId) {

                snd.retries = 0;
                snd.queueArn = originalQueue.arn;
                snd.queueName = originalQueue.name;
                snd.status = Entity::SQS::MessageStatus::INITIAL;
                _messages[fst] = snd;
                count++;
            }
        }
        log_trace << "Message redrive, arn: " << dlqQueue.arn << ", messageId: " << messageId;
        return count;
    }

    long SQSMemoryRepository::messageRetention(const std::string &queueArn, const long retentionPeriod) const {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        long count = 0;
        for (auto &val: _messages | std::views::values) {

            if (val.queueArn == queueArn && val.status == Entity::SQS::MessageStatus::DELAYED && val.reset < system_clock::now()) {
                count += deleteMessage(val);
            }
        }
        log_trace << "Message retention reset, deleted: " << count << " queueArn: " << queueArn;
        return count;
    }

    long SQSMemoryRepository::countMessages(const std::string &queueArn, const std::string &prefix) const {
        if (queueArn.empty()) {
            return static_cast<long>(_messages.size());
        }
        return std::ranges::count_if(_messages, [queueArn](const auto &pair) {
            return pair.second.queueArn == queueArn;
        });
    }

    Entity::SQS::MessageWaitTime SQSMemoryRepository::getAverageMessageWaitingTime() const {

        Entity::SQS::MessageWaitTime waitTime{};
        for (const Entity::SQS::QueueList queueList = listQueues(); const auto &queue: queueList) {

            // Extract map values
            std::vector<Entity::SQS::Message> filtered;
            std::ranges::transform(_messages,
                                   std::back_inserter(filtered),
                                   [queue](auto &kv) {
                                       if (kv.second.queueArn == queue.arn) {
                                           return kv.second;
                                       }
                                       return Entity::SQS::Message();
                                   });

            if (!filtered.empty()) {

                // Sort by created timestamp
                std::ranges::sort(filtered, [](const auto &x, const auto &y) { return x.created > y.created; });

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

    void SQSMemoryRepository::importMessages(const std::string &queueArn, const value &messageArray) const {
        // TODO: convert from BSON to object and import
    }

    long SQSMemoryRepository::deleteMessages(const std::string &queueArn) const {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        const auto count = std::erase_if(_messages, [queueArn](const auto &item) {
            return item.second.queueArn == queueArn;
        });

        log_debug << "Messages deleted, queueArn: " << queueArn << " count: " << count;
        return static_cast<long>(count);
    }

    long SQSMemoryRepository::deleteMessage(const Entity::SQS::Message &message) const {
        return deleteMessage(message.receiptHandle);
    }

    long SQSMemoryRepository::deleteMessage(const std::string &receiptHandle) const {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        const auto count = std::erase_if(_messages, [receiptHandle](const std::pair<std::string, Entity::SQS::Message> &item) {
            return item.second.receiptHandle == receiptHandle;
        });
        log_debug << "Messages deleted, receiptHandle: " << receiptHandle << " count: " << count;
        return static_cast<long>(count);
    }

    long SQSMemoryRepository::deleteAllMessages() const {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        const long count = static_cast<long>(_messages.size());
        _messages.clear();
        log_debug << "All messages deleted, count: " << count;
        return count;
    }

    long SQSMemoryRepository::countMessagesByStatus(const std::string &queueArn, const Entity::SQS::MessageStatus &status) const {
        return std::ranges::count_if(_messages, [queueArn, status](const auto &pair) {
            return pair.second.queueArn == queueArn && pair.second.status == status;
        });
    }

    void SQSMemoryRepository::adjustMessageCounters() const {
        boost::mutex::scoped_lock lock(_sqsMessageMutex);

        for (const auto &queue: _queues | std::views::values) {
            _queues[queue.oid].attributes.approximateNumberOfMessages = countMessagesByStatus(queue.arn, Entity::SQS::MessageStatus::INITIAL);
            _queues[queue.oid].attributes.approximateNumberOfMessagesNotVisible = countMessagesByStatus(queue.arn, Entity::SQS::MessageStatus::INVISIBLE);
            _queues[queue.oid].attributes.approximateNumberOfMessagesDelayed = countMessagesByStatus(queue.arn, Entity::SQS::MessageStatus::DELAYED);
        }
        log_debug << "All message counters updated, count: " << _queues.size();
    }

}// namespace Awsmock::Database
