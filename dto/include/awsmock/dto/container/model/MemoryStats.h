//
// Created by vogje01 on 06/06/2023.
//

#pragma once

// AwsMock includes
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::Docker {

    /**
     * @brief Docker container memory statistics
     *
     * @par
     * Contains statistical data from the docker daemon about one container.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct MemoryStats final : Common::BaseObject<MemoryStats> {

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

} // namespace Awsmock::Dto::Docker
