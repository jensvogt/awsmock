//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DB_ENTITY_SNS_MESSAGE_STATUS_H
#define AWSMOCK_DB_ENTITY_SNS_MESSAGE_STATUS_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Database::Entity::SNS {

    /**
     * @brief SNS message attribute entity
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum MessageStatus {
        INITIAL,
        SEND,
        RESEND
    };

    static std::map<MessageStatus, std::string> MessageStatusNames{
        {INITIAL, "INITIAL"},
        {SEND, "SEND"},
        {RESEND, "RESEND"},
    };

    [[maybe_unused]]
    static std::string MessageStatusToString(const MessageStatus &messageStatus) {
        return MessageStatusNames[messageStatus];
    }

    [[maybe_unused]]
    static MessageStatus MessageStatusFromString(const std::string &s) {
        const auto it = std::ranges::find_if(MessageStatusNames, [&](const auto &p) { return p.second == s; });
        return it != MessageStatusNames.end() ? it->first : INITIAL;
    }

} // namespace AwsMock::Database::Entity::SNS

#endif// AWSMOCK_DB_ENTITY_SNS_MESSAGE_STATUS_H
