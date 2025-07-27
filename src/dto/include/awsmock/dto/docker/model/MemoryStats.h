//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_MEMORY_STATS_H
#define AWSMOCK_DTO_DOCKER_MEMORY_STATS_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief Docker container memory statistics
     *
     * @par
     * Contains statistical data from the docker daemon about one container.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MemoryStats final : Common::BaseCounter<MemoryStats> {

        /**
         * Cached memory
         */
        long cache{};

        /**
         * Residual memory
         */
        long rss{};

      private:

        friend MemoryStats tag_invoke(boost::json::value_to_tag<MemoryStats>, boost::json::value const &v) {
            MemoryStats r;
            r.cache = Core::Json::GetLongValue(v, "cache");
            r.rss = Core::Json::GetLongValue(v, "rss");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MemoryStats const &obj) {
            jv = {
                    {"cache", obj.cache},
                    {"rss", obj.rss},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_MEMORY_STATS_H
