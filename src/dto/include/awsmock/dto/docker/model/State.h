//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_STATE_H
#define AWSMOCK_DTO_DOCKER_STATE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

#define LAMBDA_INTERNAL_PORT 8080

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker container state
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct State final : Common::BaseCounter<State> {

        /**
         * Error
         */
        std::string error;

        /**
         * Error code
         */
        int errorCode{};

        /**
         * PID
         */
        int pid{};

        /**
         * Running state
         */
        bool running{};

        /**
         * Dead state
         */
        bool dead{};

        /**
         * Paused state
         */
        bool paused{};

        /**
         * Restarting state
         */
        bool restarting{};

        /**
         * OOM killed state
         */
        bool oomKilled{};

        /**
         * Status
         */
        std::string status;

      private:

        friend State tag_invoke(boost::json::value_to_tag<State>, boost::json::value const &v) {
            State r;
            r.status = Core::Json::GetStringValue(v, "Status");
            r.error = Core::Json::GetStringValue(v, "Error");
            r.errorCode = Core::Json::GetIntValue(v, "ErrorCode");
            r.pid = Core::Json::GetIntValue(v, "Pid");
            r.running = Core::Json::GetBoolValue(v, "Running");
            r.dead = Core::Json::GetBoolValue(v, "Dead");
            r.paused = Core::Json::GetBoolValue(v, "Paused");
            r.restarting = Core::Json::GetBoolValue(v, "Restarting");
            r.oomKilled = Core::Json::GetBoolValue(v, "OOMKilled");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, State const &obj) {
            jv = {
                    {"Status", obj.status},
                    {"Error", obj.error},
                    {"ErrorCode", obj.errorCode},
                    {"Pid", obj.pid},
                    {"Running", obj.running},
                    {"Dead", obj.dead},
                    {"Paused", obj.paused},
                    {"Restarting", obj.restarting},
                    {"OOMKilled", obj.oomKilled},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_STATE_H
