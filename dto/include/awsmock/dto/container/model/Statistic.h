//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_STATISTIC_H
#define AWSMOCK_DTO_DOCKER_STATISTIC_H

// C++ includes
#include <string>

// AwsMock includes
#include "CpuStat.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

#define LAMBDA_INTERNAL_PORT 8080

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker container statistics
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Statistic final : Common::BaseCounter<Statistic> {

        /**
         * Container ID
         */
        std::string containerId;

        /**
         * Container name
         */
        std::string name;

        /**
         * OS type
         */
        std::string osType;

        /**
         * Read timestamp
         */
        system_clock::time_point readTime;

        /**
         * CPU statistics
         */
        CpuStat cpuStats;

      private:

        friend Statistic tag_invoke(boost::json::value_to_tag<Statistic>, boost::json::value const &v) {
            Statistic r;
            r.containerId = Core::Json::GetStringValue(v, "containerId");
            r.name = Core::Json::GetStringValue(v, "name");
            r.osType = Core::Json::GetStringValue(v, "os_type");
            r.readTime = Core::DateTimeUtils::FromISO8601(Core::Json::GetStringValue(v, "read"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Statistic const &obj) {
            jv = {
                    {"containerId", obj.containerId},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_STATISTIC_H
