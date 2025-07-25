//
// Created by vogje01 on 6/5/24.
//

#ifndef AWSMOCK_DTO_APPS_WEBSOCKET_COMMAND_H
#define AWSMOCK_DTO_APPS_WEBSOCKET_COMMAND_H

// C++ includes
#include <map>
#include <string>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Web socket command types
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class WebSoketCommandType {
        OPEN_LOG,
        LOG_MESSAGE,
        CLOSE_LOG,
        OPEN_AWSMOCK_LOGS,
        UNKNOWN
    };

    static std::map<WebSoketCommandType, std::string> WebSocketCommandTypeNames{
            {WebSoketCommandType::OPEN_LOG, "open-log"},
            {WebSoketCommandType::LOG_MESSAGE, "log-message"},
            {WebSoketCommandType::CLOSE_LOG, "close-log"},
            {WebSoketCommandType::OPEN_AWSMOCK_LOGS, "open-awsmock-logs"},
            {WebSoketCommandType::UNKNOWN, "unknown"},
    };

    [[maybe_unused]] static std::string WebSocketCommandTypeToString(const WebSoketCommandType &websocketCommand) {
        return WebSocketCommandTypeNames[websocketCommand];
    }

    [[maybe_unused]] static WebSoketCommandType WebSocketCommandTyeFromString(const std::string &websocketCommand) {
        for (auto &[fst, snd]: WebSocketCommandTypeNames) {
            if (snd == websocketCommand) {
                return fst;
            }
        }
        return WebSoketCommandType::UNKNOWN;
    }

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_WEBSOCKET_COMMAND_H
