//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_MEMORY_STAT_H
#define AWSMOCK_DTO_DOCKER_MEMORY_STAT_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/docker/model/MemoryStats.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker container memory statistics
     *
     * @par
     * Contains statistical data from the docker daemon about one container.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MemoryStat final : Common::BaseCounter<MemoryStat> {

        /**
         * Maximal usage in bytes
         */
        long maxUsage{};

        /**
         * Current usage in bytes
         */
        long usage{};

        /**
         * Limit in bytes
         */
        long limit{};

        /**
         * Stats
         */
        MemoryStats stats;

      private:

        friend MemoryStat tag_invoke(boost::json::value_to_tag<MemoryStat>, boost::json::value const &v) {
            MemoryStat r;
            r.maxUsage = Core::Json::GetLongValue(v, "max_usage");
            r.usage = Core::Json::GetLongValue(v, "usage");
            r.limit = Core::Json::GetLongValue(v, "limit");
            if (Core::Json::AttributeExists(v, "stats")) {
                r.stats = boost::json::value_to<MemoryStats>(v.at("stats"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MemoryStat const &obj) {
            jv = {
                    {"max_usage", obj.maxUsage},
                    {"usage", obj.usage},
                    {"limit", obj.limit},
                    {"stats", boost::json::value_from(obj.stats)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_CONTAINER_H
