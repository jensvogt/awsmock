//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_UPDATE_LAMBDA_CONFIGURATION_REQUEST_H
#define AWSMOCK_DTO_S3_UPDATE_LAMBDA_CONFIGURATION_REQUEST_H

// C++ Standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/SortColumn.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/s3/model/LambdaConfiguration.h>

namespace AwsMock::Dto::S3 {

    struct UpdateLambdaConfigurationRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Lambda configuration
         */
        LambdaConfiguration lambdaConfiguration;

        /**
         * @brief Parse values from a JSON stream
         *
         * @param body json input stream
         */
        void FromJson(const std::string &body);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const UpdateLambdaConfigurationRequest &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_UPDATE_LAMBDA_CONFIGURATION_REQUEST_H
