//
// Created by vogje01 on 11/19/23.
//

#include <awsmock/memorydb/SNSMemoryDb.h>

namespace AwsMock::Database {

    boost::mutex SNSMemoryDb::_snsTopicMutex;
    boost::mutex SNSMemoryDb::_snsMessageMutex;

    bool SNSMemoryDb::TopicExists(const std::string &region, const std::string &name) {

        return std::ranges::find_if(_topics,
                                    [region, name](const std::pair<std::string, Entity::SNS::Topic> &topic) {
                                        return topic.second.region == region && topic.second.topicName == name;
                                    }) != _topics.end();
    }

    bool SNSMemoryDb::TopicExists(const std::string &arn) {

        return std::ranges::find_if(_topics, [arn](const std::pair<std::string, Entity::SNS::Topic> &topic) {
                   return topic.second.topicArn == arn;
               }) != _topics.end();
    }

    Entity::SNS::Topic SNSMemoryDb::GetTopicById(const std::string &oid) {

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

    Entity::SNS::Topic SNSMemoryDb::GetTopicByArn(const std::string &topicArn) {

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

    Entity::SNS::Topic SNSMemoryDb::GetTopicByName(const std::string &region, const std::string &topicName) {

        const auto it =
                std::ranges::find_if(_topics, [region, topicName](const std::pair<std::string, Entity::SNS::Topic> &topic) {
                    return topic.second.region == region && topic.second.topicName == topicName;
                });

        if (it != _topics.end()) {
            it->second.oid = it->first;
            return it->second;
        }

        log_warning << "Topic not found, region: " << region << " name: " << topicName;
        return {};
    }

    Entity::SNS::TopicList SNSMemoryDb::GetTopicsBySubscriptionArn(const std::string &subscriptionArn) {

        Entity::SNS::TopicList topics;
        for (const auto &topic: _topics) {
            if (!topic.second.subscriptions.empty()) {
                auto it = std::ranges::find_if(topic.second.subscriptions,
                                               [subscriptionArn](const Entity::SNS::Subscription &subcription) {
                                                   return subcription.subscriptionArn == subscriptionArn;
                                               });
                if (it != topic.second.subscriptions.end()) {
                    topics.emplace_back(topic.second);
                }
            }
        }
        return topics;
    }

    Entity::SNS::Topic SNSMemoryDb::CreateTopic(const Entity::SNS::Topic &topic) {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _topics[oid] = topic;
        log_trace << "Topic created, oid: " << oid;
        return GetTopicById(oid);
    }

    Entity::SNS::Topic SNSMemoryDb::UpdateTopic(Entity::SNS::Topic &topic) {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        topic.modified = system_clock::now();

        std::string region = topic.region;
        std::string name = topic.topicName;
        const auto it = std::ranges::find_if(_topics,
                                             [region, name](const std::pair<std::string, Entity::SNS::Topic> &topic) {
                                                 return topic.second.region == region && topic.second.topicName == name;
                                             });
        _topics[it->first] = topic;
        return _topics[it->first];
    }

    Entity::SNS::TopicList SNSMemoryDb::ListTopics(const std::string &region) const {

        Entity::SNS::TopicList topicList;
        if (region.empty()) {

            for (const auto &topic: _topics) {
                topicList.emplace_back(topic.second);
            }

        } else {

            for (const auto &topic: _topics) {
                if (topic.second.region == region) {
                    topicList.emplace_back(topic.second);
                }
            }
        }

        log_trace << "Got topic list, size: " << topicList.size();
        return topicList;
    }

    long SNSMemoryDb::CountTopics(const std::string &region) const {

        long count = 0;
        if (region.empty()) {

            count = static_cast<long>(_topics.size());

        } else {

            for (const auto &topic: _topics) {
                if (topic.second.region == region) {
                    count++;
                }
            }
        }
        return count;
    }

    void SNSMemoryDb::PurgeTopic(const Entity::SNS::Topic &topic) {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        const auto count = std::erase_if(_messages, [topic](const auto &item) {
            auto const &[key, value] = item;
            return value.topicArn == topic.topicArn;
        });
        log_debug << "Topic purged, count: " << count;
    }

    long SNSMemoryDb::GetTopicSize(const std::string &topicArn) const {

        long sum = 0;
        std::for_each(_messages.rbegin(), _messages.rend(), [&](const std::pair<std::string, Entity::SNS::Message> &m) {
            if (m.second.topicArn == topicArn) {
                sum += m.second.size;
            }
        });
        log_trace << "Sum size, arn: " << topicArn << " sum: " << sum;
        return sum;
    }

    void SNSMemoryDb::DeleteTopic(const Entity::SNS::Topic &topic) {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        std::string region = topic.region;
        std::string arn = topic.topicArn;
        const auto count = std::erase_if(_topics, [region, arn](const auto &item) {
            auto const &[key, value] = item;
            return value.region == region && value.topicArn == arn;
        });
        log_debug << "Topic deleted, count: " << count;
    }

    void SNSMemoryDb::DeleteAllTopics() {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        log_debug << "All topics deleted, count: " << _topics.size();
        _topics.clear();
    }

    bool SNSMemoryDb::MessageExists(const std::string &id) {

        return std::ranges::find_if(_messages, [id](const std::pair<std::string, Entity::SNS::Message> &message) {
                   return message.first == id;
               }) != _messages.end();
    }

    Entity::SNS::Message SNSMemoryDb::CreateMessage(const Entity::SNS::Message &message) {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        const std::string oid = Core::StringUtils::CreateRandomUuid();
        _messages[oid] = message;
        log_trace << "Message created, oid: " << oid;
        return GetMessageById(oid);
    }

    Entity::SNS::Message SNSMemoryDb::GetMessageById(const std::string &oid) {

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

    long SNSMemoryDb::CountMessages(const std::string &topicArn) const {

        if (topicArn.empty()) {
            return _messages.size();
        }

        long count = 0;
        for (const auto &message: _messages) {
            if (!topicArn.empty() && message.second.topicArn == topicArn) {
                count++;
            }
        }
        return count;
    }

    long SNSMemoryDb::CountMessagesByStatus(const std::string &region, const std::string &topicArn, const Entity::SNS::MessageStatus status) const {

        long count = 0;
        for (const auto &message: _messages) {
            if (!region.empty() && message.second.region == region && !topicArn.empty() && message.second.topicArn == topicArn && message.second.status == status) {
                count++;
            }
        }
        return count;
    }

    Entity::SNS::MessageList SNSMemoryDb::ListMessages(const std::string &region, const std::string &topicArn) {

        Entity::SNS::MessageList messageList;
        if (region.empty() && topicArn.empty()) {

            for (const auto &message: _messages) {
                messageList.emplace_back(message.second);
            }

        } else if (topicArn.empty()) {

            for (const auto &message: _messages) {
                if (message.second.region == region) {
                    messageList.emplace_back(message.second);
                }
            }
        } else if (region.empty()) {

            for (const auto &message: _messages) {
                if (message.second.topicArn == topicArn) {
                    messageList.emplace_back(message.second);
                }
            }
        } else {
            for (const auto &message: _messages) {
                if (message.second.region == region && message.second.topicArn == topicArn) {
                    messageList.emplace_back(message.second);
                }
            }
        }

        log_trace << "Got message list, size: " << messageList.size();
        return messageList;
    }

    Entity::SNS::Message SNSMemoryDb::UpdateMessage(Entity::SNS::Message &message) {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        message.modified = system_clock::now();

        std::string oid = message.oid;
        const auto it =
                std::ranges::find_if(_messages, [oid](const std::pair<std::string, Entity::SNS::Message> &message) {
                    return message.second.oid == oid;
                });
        _messages[it->first] = message;
        return _messages[it->first];
    }

    void SNSMemoryDb::DeleteMessage(const Entity::SNS::Message &message) {
        DeleteMessage(message.messageId);
    }

    void SNSMemoryDb::DeleteMessage(const std::string &messageId) {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        const auto count = std::erase_if(_messages, [messageId](const auto &item) {
            auto const &[key, value] = item;
            return value.messageId == messageId;
        });
        log_debug << "Message deleted, messageId: " << messageId << " count: " << count;
    }

    void SNSMemoryDb::DeleteMessages(const std::string &region, const std::string &topicArn, const std::vector<std::string> &messageIds) {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        long count = 0;
        for (auto &messageId: messageIds) {
            count += static_cast<long>(std::erase_if(_messages, [region, topicArn, messageId](const auto &item) {
                auto const &[key, value] = item;
                return value.region == region && value.topicArn == topicArn && value.messageId == messageId;
            }));
        }
        log_debug << "Messages deleted, count: " << count;
    }

    void SNSMemoryDb::DeleteOldMessages(long timeout) {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        auto reset = std::chrono::high_resolution_clock::now() - std::chrono::seconds{timeout};

        long count = 0;
        for ([[maybe_unused]] auto &messageId: _messages) {
            count += static_cast<long>(std::erase_if(_messages, [reset](const auto &item) {
                auto const &[key, value] = item;
                return value.created < reset;
            }));
        }
        log_debug << "Old resources deleted, timeout: " << timeout << " count: " << count;
    }

    void SNSMemoryDb::DeleteAllMessages() {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        log_debug << "All resources deleted, count: " << _messages.size();
        _messages.clear();
    }
}// namespace AwsMock::Database