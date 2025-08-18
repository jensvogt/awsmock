//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_MOVEOBJECTSRESPONSE_H
#define AWSMOCK_DTO_S3_MOVEOBJECTSRESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    struct MoveObjectResponse {

        /**
         * Etag
         */
        std::string eTag;

        /**
         * Last modified
         */
        std::string lastModified;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
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
        friend std::ostream &operator<<(std::ostream &os, const MoveObjectResponse &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_MOVEOBJECTSRESPONSE_H
