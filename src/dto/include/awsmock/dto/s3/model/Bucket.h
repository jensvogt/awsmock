// Created by vogje01 on 10/1/24.
//

#ifndef AWSMOCK_DTO_S3_MODEL_BUCKET_H
#define AWSMOCK_DTO_S3_MODEL_BUCKET_H

// C++ includes
#include <chrono>
#include <sstream>
#include <string>
#include <vector>

// AwsMock includes
#include "awsmock/core/JsonUtils.h"
#include "awsmock/core/LogStream.h"
#include "awsmock/core/XmlUtils.h"
#include "awsmock/core/exception/JsonException.h"
#include <awsmock/dto/s3/model/LambdaConfiguration.h>
#include <awsmock/dto/s3/model/ObjectVersion.h>
#include <awsmock/dto/s3/model/QueueConfiguration.h>
#include <awsmock/dto/s3/model/TopicConfiguration.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    struct Bucket {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Bucket name
         */
        std::string bucketName;

        /**
         * Owner
         */
        std::string owner;

        /**
         * ARM
         */
        std::string arn;

        /**
         * Number of objects keys
         */
        long keys;

        /**
         * Bucket size in bytes
         */
        long size;

        /**
         * Version status
         */
        std::string versionStatus;

        /**
         * List of queue notifications
         */
        std::vector<Dto::S3::QueueConfiguration> queueConfigurations;

        /**
         * List of topic notifications
         */
        std::vector<Dto::S3::TopicConfiguration> topicConfigurations;

        /**
         * List of lambda notifications
         */
        std::vector<Dto::S3::LambdaConfiguration> lambdaConfigurations;

        /**
         * Create timestamp
         */
        system_clock::time_point created;

        /**
         * Modified timestamp
         */
        system_clock::time_point modified;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        void FromJsonObject(const Poco::JSON::Object::Ptr &jsonObject);

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] Poco::JSON::Object ToJsonObject() const;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const Bucket &r);
    };

}// namespace AwsMock::Dto::S3

#endif//AWSMOCK_DTO_S3_MODEL_BUCKET_H