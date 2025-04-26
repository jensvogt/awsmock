//
// Created by vogje01 on 01/06/2023.
//

#ifndef AWSMOCK_DTO_SNS_MESSAGE_STATUS_H
#define AWSMOCK_DTO_SNS_MESSAGE_STATUS_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::SNS {

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

    [[maybe_unused]] static std::string MessageStatusToString(const MessageStatus &messageStatus) {
        return MessageStatusNames[messageStatus];
    }

    [[maybe_unused]] static MessageStatus MessageStatusFromString(const std::string &messageStatusString) {
        for (auto &[fst, snd]: MessageStatusNames) {
            if (snd == messageStatusString) {
                return fst;
            }
        }
        return INITIAL;
    }

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_MESSAGE_STATUS_H
