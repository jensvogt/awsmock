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
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/docker/model/Image.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief List images response
     * 
     * @code{.json}
     * [
     *  {
     *    "Containers" : -1,
     *    "Created" : 1734203923,
     *    "Id" : "sha256:cda5cba92d2463d73daaac5451a0e151d06ca8df8853eed5c260d2cb1be77fd4",
     *    "Labels" : {
     *      "com.amazonaws.lambda.platform.kernel" : "k510ga"
     *     },
     *    "ParentId" : "sha256:97592e3290eeb74fc5106edb4a4d6a2df8b5ef7c1fa5f0caea806fbfbf2ca007",
     *    "RepoDigests" : [ ],
     *    "RepoTags" : [ "ftp-file-copy:latest" ],
     *    "SharedSize" : -1,
     *    "Size" : 448836289
     *   }
     * ]
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListImageResponse {

        /**
         * Image list
         */
        std::vector<Image> imageList;

        /**
         * @brief Convert to a JSON string
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Convert to a JSON string
         *
         * @return object JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ListImageResponse &r);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_LIST_IMAGE_RESPONSE_H
