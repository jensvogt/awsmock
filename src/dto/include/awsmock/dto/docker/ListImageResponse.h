//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_LIST_IMAGE_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_LIST_IMAGE_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/docker/model/Image.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief List images response
     * 
     * @code{.json}
     * [
     *  {
     *    "Containers": -1,
     *    "Created": 1734203923,
     *    "Id": "sha256:cda5cba92d2463d73daaac5451a0e151d06ca8df8853eed5c260d2cb1be77fd4",
     *    "Labels": {
     *      "com.amazonaws.lambda.platform.kernel": "k510ga"
     *     },
     *    "ParentId": "sha256:97592e3290eeb74fc5106edb4a4d6a2df8b5ef7c1fa5f0caea806fbfbf2ca007",
     *    "RepoDigests": [ ],
     *    "RepoTags": [ "ftp-file-copy:latest" ],
     *    "SharedSize": -1,
     *    "Size": 448836289
     *   }
     * ]
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListImageResponse final : Common::BaseCounter<ListImageResponse> {

        /**
         * Image list
         */
        std::vector<Image> imageList;

      private:

        friend ListImageResponse tag_invoke(boost::json::value_to_tag<ListImageResponse>, boost::json::value const &v) {
            ListImageResponse r;
            r.imageList = boost::json::value_to<std::vector<Image>>(v);
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ListImageResponse const &obj) {
            jv = {
                    {boost::json::value_from(obj.imageList)},
            };
        }
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_LIST_IMAGE_RESPONSE_H
