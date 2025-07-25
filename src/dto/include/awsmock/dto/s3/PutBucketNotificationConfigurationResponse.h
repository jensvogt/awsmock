//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_RESPONSE_H
#define AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_RESPONSE_H

// C++ standard includes
#include <string>
#include <vector>

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

    /**
     * @brief S3 Notification configuration
     *
     * Example:
     * @code{.xml}
     * <NotificationConfiguration>
     *   <TopicConfiguration>
     *     <Topic>arn:aws:sns:us-east-1:356671443308:s3notificationtopic2</Topic>
     *     <Event>s3:ReducedRedundancyLostObject</Event>
     *   </TopicConfiguration>
     *   <QueueConfiguration>
     *      <Queue>arn:aws:sqs:us-east-1:356671443308:s3notificationqueue</Queue>
     *      <Event>s3:ObjectCreated:*</Event>
     *   </QueueConfiguration>
     * </NotificationConfiguration>
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PutBucketNotificationConfigurationResponse {

        /**
         * Topic configurations
         */
        std::vector<TopicConfiguration> topicConfigurations;

        /**
         * SQS queue configurations
         */
        std::vector<QueueConfiguration> queueConfigurations;

        /**
         * Lambda function configurations
         */
        std::vector<LambdaConfiguration> lambdaConfigurations;

        /**
          * @brief Convert from XML representation
          *
          * @return xmlString XML string
          */
        [[nodiscard]] std::string ToXml();

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
        friend std::ostream &operator<<(std::ostream &os, const PutBucketNotificationConfigurationResponse &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_RESPONSE_H
