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

    Entity::SNS::Topic SNSMemoryDb::GetTopicByTargetArn(const std::string &targetArn) {

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

    Entity::SNS::TopicList SNSMemoryDb::GetTopicsBySubscriptionArn(const std::string &subscriptionArn) const {

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
                                             [region, name](const std::pair<std::string, Entity::SNS::Topic> &t) {
                                                 return t.second.region == region && t.second.topicName == name;
                                             });
        _topics[it->first] = topic;
        return _topics[it->first];
    }

    Entity::SNS::TopicList SNSMemoryDb::ListTopics(const std::string &region) const {

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

    Entity::SNS::TopicList SNSMemoryDb::ExportTopics(const std::vector<SortColumn> &sortColumns) const {

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

    long SNSMemoryDb::CountTopics(const std::string &region) const {

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

    long SNSMemoryDb::PurgeTopic(const Entity::SNS::Topic &topic) {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        const auto count = std::erase_if(_messages, [topic](const auto &item) {
            auto const &[key, value] = item;
            return value.topicArn == topic.topicArn;
        });
        log_debug << "Topic purged, count: " << count;
        return static_cast<long>(count);
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

    void SNSMemoryDb::UpdateTopicCounter(const std::string &topicArn, const long messages, const long size, long initial, const long send, const long resend) {
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

    long SNSMemoryDb::DeleteAllTopics() {
        boost::mutex::scoped_lock lock(_snsTopicMutex);

        const long count = _topics.size();
        log_debug << "All topics deleted, count: " << _topics.size();
        _topics.clear();
        return count;
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
            return static_cast<long>(_messages.size());
        }

        long count = 0;
        for (const auto &val: _messages | std::views::values) {
            if (!topicArn.empty() && val.topicArn == topicArn) {
                count++;
            }
        }
        return count;
    }

    long SNSMemoryDb::CountMessagesByStatus(const std::string &topicArn, const Entity::SNS::MessageStatus status) const {

        long count = 0;
        for (const auto &val: _messages | std::views::values) {
            if (!topicArn.empty() && val.topicArn == topicArn && val.status == status) {
                count++;
            }
        }
        return count;
    }

    Entity::SNS::MessageList SNSMemoryDb::ListMessages(const std::string &region, const std::string &topicArn) const {

        Entity::SNS::MessageList messageList;
        if (region.empty() && topicArn.empty()) {

            for (const auto &val: _messages | std::views::values) {
                messageList.emplace_back(val);
            }

        } else if (topicArn.empty()) {

            for (const auto &val: _messages | std::views::values) {
                if (val.region == region) {
                    messageList.emplace_back(val);
                }
            }
        } else if (region.empty()) {

            for (const auto &val: _messages | std::views::values) {
                if (val.topicArn == topicArn) {
                    messageList.emplace_back(val);
                }
            }
        } else {
            for (const auto &val: _messages | std::views::values) {
                if (val.region == region && val.topicArn == topicArn) {
                    messageList.emplace_back(val);
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

    void SNSMemoryDb::SetMessageStatus(const Entity::SNS::Message &message, const Entity::SNS::MessageStatus &status) {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        std::string oid = message.oid;
        const auto it =
                std::ranges::find_if(_messages, [oid](const std::pair<std::string, Entity::SNS::Message> &message) {
                    return message.second.oid == oid;
                });
        _messages[it->first].status = status;
        _messages[it->first].modified = system_clock::now();
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

    void SNSMemoryDb::DeleteOldMessages(const long timeout) {
        boost::mutex::scoped_lock lock(_snsMessageMutex);

        auto reset = system_clock::now() - std::chrono::seconds{timeout};

        long count = 0;
        for ([[maybe_unused]] auto &messageId: _messages) {
            count += static_cast<long>(std::erase_if(_messages, [reset](const auto &item) {
                auto const &[key, value] = item;
                return value.created < reset;
            }));
        }
        log_debug << "Old resources deleted, timeout: " << timeout << " count: " << count;
    }

    long SNSMemoryDb::DeleteAllMessages() {
        boost::mutex::scoped_lock lock(_snsMessageMutex);
        const long deleted = static_cast<long>(_messages.size());
        log_debug << "All resources deleted, count: " << _messages.size();
        _messages.clear();
        return deleted;
    }
}// namespace AwsMock::Database