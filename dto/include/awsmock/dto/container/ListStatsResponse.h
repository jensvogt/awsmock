//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_LIST_STATS_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_LIST_STATS_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/HttpUtils.h>
#include <awsmock/dto/container/model/Statistic.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief List container statistics response
     *
     * @code{.json}
     * [ {
     *     "Id" : string,
     *     "Name" : string,
     *     "Image" : string,
     *     "ImageID" : string,
     *     "CpuPercent" : number,
     *     "Created" : 1760866109,
     *   }
     *   ...
     * ]
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListStatsResponse final : Common::BaseCounter<ListStatsResponse> {

        /**
         * Container list
         */
        std::vector<Statistic> statistics;

      private:

        friend ListStatsResponse tag_invoke(boost::json::value_to_tag<ListStatsResponse>, boost::json::value const &v) {
            ListStatsResponse r;
            r.statistics = boost::json::value_to<std::vector<Statistic>>(v);
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListStatsResponse const &obj) {
            jv = {
                    {boost::json::value_from(obj.statistics)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_LIST_STATS_RESPONSE_H
