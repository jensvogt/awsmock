//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_APPS_WEB_SOCKET_COMMAND_H
#define AWSMOCK_DTO_APPS_WEB_SOCKET_COMMAND_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/dto/apps/model/Application.h>
#include <awsmock/dto/apps/model/WebSocketCommand.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Apps {

    /**
     * @brief Update an application
     *
     * @par
     * Request to update an  application.
     *
     * Example:
     * @code{.json}
     * {
     *   "application":
     *   {
     *     "name": "string",
     *     "runtime": "string",
     *     "runType": "string",
     *     "privatePort": number,
     *     "archive": number,
     *     "enabled": "string",
     *     "environment": [{
     *        "key": "string",
     *        "value": "string",
     *        ...
     *     }],
     *     "tags": [{
     *        "key": "string",
     *        "value": "string",
     *        ...
     *     }],
     *     "options": [{
     *        "key": "string",
     *        "value": "string",
     *        ...
     *     }],
     *   }
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct WebSocketCommand final : Common::BaseCounter<WebSocketCommand> {

        /**
         * Web socket command
         */
        WebSoketCommandType command = WebSoketCommandType::UNKNOWN;

        /**
         * Application name
         */
        std::string applicationName;

        /**
         * Container ID
         */
        std::string containerId;

      private:

        friend WebSocketCommand tag_invoke(boost::json::value_to_tag<WebSocketCommand>, boost::json::value const &v) {
            WebSocketCommand r;
            r.command = WebSocketCommandTyeFromString(Core::Json::GetStringValue(v, "command"));
            r.applicationName = Core::Json::GetStringValue(v, "applicationName");
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, WebSocketCommand const &obj) {
            jv = {
                    {"command", WebSocketCommandTypeToString(obj.command)},
                    {"applicationName", obj.applicationName},
                    {"containerId", obj.containerId},
            };
        }
    };

}// namespace AwsMock::Dto::Apps

#endif// AWSMOCK_DTO_APPS_START_APPLICATION_REQUEST_H
