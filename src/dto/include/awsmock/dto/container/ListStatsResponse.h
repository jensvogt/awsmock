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
     * @brief List container request
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
    struct ListContainerResponse final : Common::BaseCounter<ListContainerResponse> {

        /**
         * Container list
         */
        std::vector<Statistic> statistics;

      private:

        friend ListContainerResponse tag_invoke(boost::json::value_to_tag<ListContainerResponse>, boost::json::value const &v) {
            ListContainerResponse r;
            r.statistics = boost::json::value_to<std::vector<Statistic>>(v);
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListContainerResponse const &obj) {
            jv = {
                    {boost::json::value_from(obj.statistics)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_LIST_STATS_RESPONSE_H
