//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SQS_MESSAGE_STATUS_H
#define AWSMOCK_DB_ENTITY_SQS_MESSAGE_STATUS_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Database::Entity::SQS {

    /**
     * SQS message attribute entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class MessageStatus {
        INITIAL,
        DELAYED,
        INVISIBLE,
        UNKNOWN
    };

    static std::map<MessageStatus, std::string> MessageStatusNames{
            {MessageStatus::INITIAL, "INITIAL"},
            {MessageStatus::DELAYED, "DELAYED"},
            {MessageStatus::INVISIBLE, "INVISIBLE"},
    };

    [[maybe_unused]] static std::string MessageStatusToString(MessageStatus messageStatus) {
        return MessageStatusNames[messageStatus];
    }

    [[maybe_unused]] static MessageStatus MessageStatusFromString(const std::string &messageStatus) {
        for (auto &it: MessageStatusNames) {
            if (it.second == messageStatus) {
                return it.first;
            }
        }
        return MessageStatus::UNKNOWN;
    }
}// namespace AwsMock::Database::Entity::SQS

#endif// AWSMOCK_DB_ENTITY_SQS_MESSAGE_STATUS_H
