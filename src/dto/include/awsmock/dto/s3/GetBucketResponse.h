//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_BUCKET_RESPONSE_H
#define AWSMOCK_DTO_S3_GET_BUCKET_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/s3/model/LambdaConfiguration.h>
#include <awsmock/dto/s3/model/QueueConfiguration.h>
#include <awsmock/dto/s3/model/TopicConfiguration.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * @brief Get bucket response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetBucketResponse {

        /**
         * ID
         */
        std::string id;

        /**
         * AWS region
         */
        std::string region;

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * AWS ARN
         */
        std::string arn;

        /**
         * Owner
         */
        std::string owner;

        /**
         * Version status
         */
        std::string versionStatus;

        /**
         * Size in bytes
         */
        long size;

        /**
         * Object keys count
         */
        long keys;

        /**
         * Lambda notification configurations
         */
        std::vector<LambdaConfiguration> lambdaConfigurations;

        /**
         * Queue notification configurations
         */
        std::vector<QueueConfiguration> queueConfigurations;

        /**
         * Queue notification configurations
         */
        std::vector<TopicConfiguration> topicConfigurations;

        /**
         * Created
         */
        system_clock::time_point created;

        /**
         * Modified
         */
        system_clock::time_point modified;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const GetBucketResponse &r);
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_GET_BUCKET_RESPONSE_H
