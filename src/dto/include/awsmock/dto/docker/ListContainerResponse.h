//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_DTO_DOCKER_LIST_CONTAINER_RESPONSE_H
#define AWSMOCK_DTO_DOCKER_LIST_CONTAINER_RESPONSE_H

// C++ includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/docker/model/Container.h>

namespace AwsMock::Dto::Docker {

    /**
     * @brief List container request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListContainerResponse {

        /**
         * Container list
         */
        std::vector<Container> containerList;

        /**
         * HTTP status
         */
        boost::beast::http::status status;

        /**
         * @brief Constructor
         */
        explicit ListContainerResponse(const std::string &body);

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
        friend std::ostream &operator<<(std::ostream &os, const ListContainerResponse &r);
    };

}// namespace AwsMock::Dto::Docker

#endif// AWSMOCK_DTO_DOCKER_LIST_CONTAINER_RESPONSE_H
