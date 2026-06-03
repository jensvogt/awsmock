//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/repository/sns/SNSMemoryRepository.h>

namespace Awsmock::Database {

    boost::mutex SNSMemoryRepository::_snsTopicMutex;
    boost::mutex SNSMemoryRepository::_snsMessageMutex;

    bool SNSMemoryRepository::topicExists(const std::string &region, const std::string &name) const {

        return std::ranges::find_if(_topics,
                                    [region, name](const std::pair<std::string, Entity::SNS::Topic> &topic) {
                                        return topic.second.region == region && topic.second.topicName == name;
                                    }) != _topics.end();
    }

    bool SNSMemoryRepository::topicExists(const std::string &topicName) const {

        return std::ranges::find_if(_topics, [topicName](const std::pair<std::string, Entity::SNS::Topic> &topic) {
                   return topic.second.topicArn == topicName;
               }) != _topics.end();
    }

    Entity::SNS::Topic SNSMemoryRepository::getTopicById(const std::string &oid) const {

        const auto it = std::ranges::find_if(_topics, [oid](const std::pair<std::string, Entity::SNS::Topic> &topic) {
            return topic.first == oid;
        });

        if (it != _topics.end()) {
            it->second.oid = oid;
            return it->second;
        }

        log_warning << "Topic not found, oid: " << oid;
        return {};
    }

    Entity::SNS::Topic SNSMemoryRepository::getTopicById(bsoncxx::oid oid) const {
        return getTopicById(oid.to_string());
    }

    Entity::SNS::Topic SNSMemoryRepository::getTopicByArn(const std::string &topicArn) const {

        const auto it =
                std::ranges::find_if(_topics, [topicArn](const std::pair<std::string, Entity::SNS::Topic> &topic) {
                    return topic.second.topicArn == topicArn;
                });

        if (it != _topics.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        log_warning << "Topic not found, topicArn: " << topicArn;
        return {};
    }

    Entity::SNS::Topic SNSMemoryRepository::getTopicByName(const std::string &region, const std::string &topicName) const {

        const auto it = std::ranges::find_if(_topics, [region, topicName](const std::pair<std::string, Entity::SNS::Topic> &topic) {
            return topic.second.region == region && topic.second.topicName == topicName;
        });

        if (it != _topics.end()) {
            it->second.oid = it->first;
            return it->second;
        }

        log_warning << "Topic not found, region: " << region << " name: " << topicName;
        return {};
    }

    Entity::SNS::Topic SNSMemoryRepository::getTopicByTargetArn(const std::string &targetArn) const {

        const auto it =
                std::ranges::find_if(_topics, [targetArn](const std::pair<std::string, Entity::SNS::Topic> &topic) {
                    return topic.second.targetArn == targetArn;
                });

        if (it != _topics.end()) {
            it->second.oid = it->first;
            return it->second;
        }
        log_warning << "Topic not found, targetArn: " << targetArn;
        return {};
    }

    Entity::SNS::TopicList SNSMemoryRepository::getTopicsBySubscriptionArn(const std::string &subscriptionArn) const {

        Entity::SNS::TopicList topics;
        for (const auto &val: _topics | std::views::values) {
            if (!val.subscriptions.empty()) {
                auto it = std::ranges::find_if(val.subscriptions,
                                               [subscriptionArn](const Entity::SNS::Subscription &subcription) {
                                                   return subcription.subscriptionArn == subscriptionArn;
                                               });
                if (it != val.subscriptions.end()) {
                    topics.emplace_back(val);
                }
            }
        }
        return topics;
    }

    Entity::SNS::Topic SNSMemoryRepository::createTopic(Entity::SNS::Topic &topic) const {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _topics[oid] = topic;
        log_trace << "Topic created, oid: " << oid;
        return _topics[oid];
    }

    Entity::SNS::Topic SNSMemoryRepository::createOrUpdateTopic(Entity::SNS::Topic &topic) const {
        if (topicExists(topic.region, topic.topicName)) {
            return updateTopic(topic);
        }
        return createTopic(topic);
    }

    Entity::SNS::Topic SNSMemoryRepository::updateTopic(Entity::SNS::Topic &topic) const {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        topic.modified = system_clock::now();

        std::string region = topic.region;
        std::string name = topic.topicName;
        const auto it = std::ranges::find_if(_topics,
                                             [region, name](const std::pair<std::string, Entity::SNS::Topic> &t) {
                                                 return t.second.region == region && t.second.topicName == name;
                                             });
        _topics[it->first] = topic;
        return _topics[it->first];
    }

    Entity::SNS::TopicList SNSMemoryRepository::listTopics(const std::string &region) const {

        Entity::SNS::TopicList topicList;
        if (region.empty()) {

            for (const auto &val: _topics | std::views::values) {
                topicList.emplace_back(val);
            }

        } else {

            for (const auto &val: _topics | std::views::values) {
                if (val.region == region) {
                    topicList.emplace_back(val);
                }
            }
        }

        log_trace << "Got topic list, size: " << topicList.size();
        return topicList;
    }

    Entity::SNS::TopicList SNSMemoryRepository::listTopics(const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns, const std::string &region) const {

        Entity::SNS::TopicList topicList;
        if (region.empty()) {

            for (const auto &val: _topics | std::views::values) {
                topicList.emplace_back(val);
            }

        } else {

            for (const auto &val: _topics | std::views::values) {
                if (val.region == region) {
                    topicList.emplace_back(val);
                }
            }
        }

        log_trace << "Got topic list, size: " << topicList.size();
        return topicList;
    }

    Entity::SNS::TopicList SNSMemoryRepository::exportTopics(const std::vector<SortColumn> &sortColumns) const {

        Entity::SNS::TopicList topicList;
        for (const auto &val: _topics | std::views::values) {
            topicList.emplace_back(val);
        }

        std::ranges::sort(topicList, [](const Entity::SNS::Topic &a, const Entity::SNS::Topic &b) {
            return a.topicName < b.topicName;
        });
        log_trace << "Got topic list, size: " << topicList.size();
        return topicList;
    }

    void SNSMemoryRepository::importTopic(Entity::SNS::Topic &topic) const {

        createOrUpdateTopic(topic);
    }

    long SNSMemoryRepository::countTopics(const std::string &region, const std::string &prefix) const {

        long count = 0;
        if (region.empty()) {

            count = static_cast<long>(_topics.size());

        } else {

            for (const auto &val: _topics | std::views::values) {
                if (val.region == region) {
                    count++;
                }
            }
        }
        return count;
    }

    long SNSMemoryRepository::purgeTopic(const Entity::SNS::Topic &topic) const {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        const auto count = std::erase_if(_messages, [topic](const auto &item) {
            auto const &[key, value] = item;
            return value.topicArn == topic.topicArn;
        });
        log_debug << "Topic purged, count: " << count;
        return static_cast<long>(count);
    }

    long SNSMemoryRepository::getTopicSize(const std::string &topicArn) const {

        long sum = 0;
        std::for_each(_messages.rbegin(), _messages.rend(), [&](const std::pair<std::string, Entity::SNS::Message> &m) {
            if (m.second.topicArn == topicArn) {
                sum += m.second.size;
            }
        });
        log_trace << "Sum size, arn: " << topicArn << " sum: " << sum;
        return sum;
    }

    void SNSMemoryRepository::updateTopicCounter(const std::string &topicArn, const long messages, const long size, long initial, const long send, const long resend) const {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        const auto it = std::ranges::find_if(_topics,
                                             [topicArn](const std::pair<std::string, Entity::SNS::Topic> &topic) {
                                                 return topic.second.topicArn == topicArn;
                                             });
        if (it != _topics.end()) {
            it->second.size = size;
            it->second.messages = messages;
        }
    }

    void SNSMemoryRepository::deleteTopic(const Entity::SNS::Topic &topic) const {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        std::string region = topic.region;
        std::string arn = topic.topicArn;
        const auto count = std::erase_if(_topics, [region, arn](const auto &item) {
            auto const &[key, value] = item;
            return value.region == region && value.topicArn == arn;
        });
        log_debug << "Topic deleted, count: " << count;
    }

    long SNSMemoryRepository::deleteAllTopics() const {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        const long count = static_cast<long>(_topics.size());
        log_debug << "All topics deleted, count: " << _topics.size();
        _topics.clear();
        return count;
    }

    bool SNSMemoryRepository::messageExists(const std::string &id) const {

        return std::ranges::find_if(_messages, [id](const std::pair<std::string, Entity::SNS::Message> &message) {
                   return message.first == id;
               }) != _messages.end();
    }

    Entity::SNS::Message SNSMemoryRepository::createMessage(Entity::SNS::Message &message) const {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _messages[oid] = message;
        log_trace << "Message created, oid: " << oid;
        return _messages[oid];
    }

    Entity::SNS::Message SNSMemoryRepository::createOrUpdateMessage(Entity::SNS::Message &message) const {
        if (messageExists(message.messageId)) {
            return updateMessage(message);
        }
        return createMessage(message);
    }

    Entity::SNS::Message SNSMemoryRepository::getMessageById(const std::string &oid) const {

        const auto it =
                std::ranges::find_if(_messages, [oid](const std::pair<std::string, Entity::SNS::Message> &message) {
                    return message.first == oid;
                });

        if (it != _messages.end()) {
            it->second.oid = oid;
            return it->second;
        }
        return {};
    }

    Entity::SNS::Message SNSMemoryRepository::getMessageById(const bsoncxx::oid oid) const {
        return getMessageById(oid.to_string());
    }

    Entity::SNS::Message SNSMemoryRepository::getMessageByMessageId(const std::string &messageId) const {

        const auto it =
                std::ranges::find_if(_messages, [messageId](const std::pair<std::string, Entity::SNS::Message> &message) {
                    return message.first == messageId;
                });

        if (it != _messages.end()) {
            return it->second;
        }
        return {};
    }

    long SNSMemoryRepository::countMessages(const std::string &topicArn) const {

        if (topicArn.empty()) {
            return static_cast<long>(_messages.size());
        }
        return std::ranges::count_if(_messages, [topicArn](const auto &pair) {
            return pair.second.topicArn == topicArn;
        });
    }

    long SNSMemoryRepository::countMessagesByStatus(const std::string &topicArn, const Entity::SNS::MessageStatus status) const {
        return std::ranges::count_if(_messages, [topicArn, status](const auto &pair) {
            return pair.second.topicArn == topicArn && pair.second.status == status;
        });
    }

    Entity::SNS::MessageList SNSMemoryRepository::listMessages(const std::string &topicArn, const std::string &prefix, long pageSize, long pageIndex, const std::vector<SortColumn> &sortColumns) const {

        auto q = Core::from(_messages | std::ranges::views::values | std::ranges::to<std::vector>());
        if (!topicArn.empty()) {
            q = q.where([topicArn](const Entity::SNS::Message &message) { return message.topicArn == topicArn; });
        }

        log_trace << "Got message list, size: " << q.count();
        return q.to_vector();
    }

    Entity::SNS::Message SNSMemoryRepository::updateMessage(Entity::SNS::Message &message) const {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        message.modified = system_clock::now();

        const auto it = std::ranges::find_if(_messages, [message](const std::pair<std::string, Entity::SNS::Message> &m) {
            return m.first == message.oid;
        });
        if (it != _messages.end()) {
            _messages[it->first] = message;
            return _messages[it->first];
        }
        return {};
    }

    void SNSMemoryRepository::setMessageStatus(const Entity::SNS::Message &message, const Entity::SNS::MessageStatus &status) const {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        const auto it = std::ranges::find_if(_messages, [message](const std::pair<std::string, Entity::SNS::Message> &m) {
            return m.first == message.oid;
        });
        if (it != _messages.end()) {
            _messages[it->first].status = status;
            _messages[it->first].modified = system_clock::now();
        }
    }

    long SNSMemoryRepository::deleteMessage(const Entity::SNS::Message &message) const {
        return deleteMessage(message.messageId);
    }

    long SNSMemoryRepository::deleteMessage(const std::string &messageId) const {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        return static_cast<long>(std::erase_if(_messages, [messageId](const auto &item) {
            auto const &[key, value] = item;
            return item.second.messageId == messageId;
        }));
    }

    long SNSMemoryRepository::deleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &messageIds) const {
        boost::mutex::scoped_lock lock(_snsMessageMutex);
        const std::unordered_set messageSet(messageIds.begin(), messageIds.end());
        return static_cast<long>(std::erase_if(_messages, [region, topicArn, messageSet](const auto &item) {
            return item.second.region == region && item.second.topicArn == topicArn && messageSet.contains(item.second.messageId);
        }));
    }

    void SNSMemoryRepository::deleteOldMessages(const long timeout) const {
        boost::mutex::scoped_lock lock(_snsMessageMutex);
        auto reset = system_clock::now() - std::chrono::seconds{timeout};
        std::erase_if(_messages, [reset](const auto &item) {
            return item.second.created < reset;
        });
    }

    long SNSMemoryRepository::deleteAllMessages() const {
        boost::mutex::scoped_lock lock(_snsMessageMutex);
        const long deleted = static_cast<long>(_messages.size());
        _messages.clear();
        log_debug << "All messages deleted, count: " << deleted;
        return deleted;
    }

    void SNSMemoryRepository::adjustMessageCounters() const {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        for (const auto &topic: _topics | std::views::values) {
            _topics[topic.oid].messages = countMessagesByStatus(topic.topicArn, Entity::SNS::MessageStatus::INITIAL);
            _topics[topic.oid].messagesSend = countMessagesByStatus(topic.topicArn, Entity::SNS::MessageStatus::SEND);
            _topics[topic.oid].messagesResend = countMessagesByStatus(topic.topicArn, Entity::SNS::MessageStatus::RESEND);
        }
        log_debug << "Topic counters updated, count: " << _topics.size();
    }
}// namespace Awsmock::Database
