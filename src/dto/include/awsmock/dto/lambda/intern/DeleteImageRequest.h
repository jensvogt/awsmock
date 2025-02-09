//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_DELETE_IMAGE_REQUEST_H
#define AWSMOCK_DTO_LAMBDA_DELETE_IMAGE_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Delete function docker image.
     *
     * @par
     * FIrst stops all container and remves containers from docker. If all containers are stopped, the image is removed from the
     * docker daemon.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteImageRequest {

        /**
         * Function ARN
         */
        std::string functionArn;

        /**
         * @brief Parse a JSON stream
         *
         * @param jsonString JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as JSON string.
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const DeleteImageRequest &r);
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_DELETE_FUNCTION_REQUEST_H
