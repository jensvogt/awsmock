//
// Created by JVO on 22.04.2024.
//

#ifndef AWSMOCK_DTO_DOCKER_IMAGE_H
#define AWSMOCK_DTO_DOCKER_IMAGE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Dto::Docker {

    /**
     * Docker image
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Image {

        /**
         * Image ID
         */
        std::string id;

        /**
         * Parent ID
         */
        std::string parentId;

        /**
         * Repository digest
         */
        std::string repoDigest;

        /**
         * Repo tags
         */
        std::vector<std::string> repoTags;

        /**
         * Shared size
         */
        long sharedSize = 0;

        /**
         * Size
         */
        long size = 0;

        /**
         * Size
         */
        long virtualSize = 0;

        /**
         * Created date time
         */
        system_clock::time_point created;

        /**
         * Repo tags
         */
        std::vector<std::string> labels;

        /**
         * Number of containers using this image
         */
        int containers = 0;

        /**
         * Convert to a JSON string
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * Convert to a JSON string
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document);

        /**
         * Convert to a JSON object
         *
         * @return object JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Image &i);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_IMAGE_H
