//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_CPU_STAT_H
#define AWSMOCK_DTO_DOCKER_CPU_STAT_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/docker/model/CpuUsage.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker container cpu statistics
     *
     * @par
     * Contains statistical data from the docker daemon about one container.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CpuStat final : Common::BaseCounter<CpuStat> {

        /**
         * Maximal usage in bytes
         */
        CpuUsage cpuUsage;

        /**
         * Number of CPUs online
         */
        long onlineCpus{};

      private:

        friend CpuStat tag_invoke(boost::json::value_to_tag<CpuStat>, boost::json::value const &v) {
            CpuStat r;
            r.cpuUsage = boost::json::value_to<CpuUsage>(v.at("cpu_usage"));
            r.onlineCpus = Core::Json::GetLongValue(v, "online_cpus");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CpuStat const &obj) {
            jv = {
                    {"cpu_usage", boost::json::value_from(obj.cpuUsage)},
                    {"online_cpus", obj.onlineCpus},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CONTAINER_H
