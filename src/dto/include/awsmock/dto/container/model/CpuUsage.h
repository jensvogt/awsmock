//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CPU_USAGE_H
#define AWSMOCK_DTO_DOCKER_CPU_USAGE_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker container cpu statistics
     *
     * @par
     * Contains statistical data from the docker daemon about one container.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CpuUsage final : Common::BaseCounter<CpuUsage> {

        /**
         * Total CPU usage
         */
        long total{};

        /**
         * User CPU usage
         */
        long user{};

        /**
         * System CPU usage
         */
        long system{};

      private:

        friend CpuUsage tag_invoke(boost::json::value_to_tag<CpuUsage>, boost::json::value const &v) {
            CpuUsage r;
            r.total = Core::Json::GetLongValue(v, "total_usage");
            r.user = Core::Json::GetLongValue(v, "usage_in_usermode");
            r.system = Core::Json::GetLongValue(v, "usage_in_kernelmode");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CpuUsage const &obj) {
            jv = {
                    {"total_usage", obj.total},
                    {"usage_in_usermode", obj.user},
                    {"usage_in_kernelmode", obj.system},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CONTAINER_H
