//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_REQUEST_H
#define AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_REQUEST_H

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
     *  <NotificationConfiguration xmlns="http://s3.amazonaws.com/doc/2006-03-01/">
     *    <TopicConfiguration>
     *      <Event>string</Event>
     *        ...
     *      <Filter>
     *        <S3Key>
     *           <FilterRule>
     *             <Name>string</Name>
     *             <Value>string</Value>
     *          </FilterRule>
     *          ...
     *        </S3Key>
     *      </Filter>
     *      <Id>string</Id>
     *      <Topic>string</Topic>
     *    </TopicConfiguration>
     *    ...
     *    <QueueConfiguration>
     *      <Event>string</Event>
     *      ...
     *      <Filter>
     *        <S3Key>
     *          <FilterRule>
     *            <Name>string</Name>
     *            <Value>string</Value>
     *          </FilterRule>
     *          ...
     *       </S3Key>
     *      </Filter>
     *      <Id>string</Id>
     *      <Queue>string</Queue>
     *    </QueueConfiguration>
     *    ...
     *    <CloudFunctionConfiguration>
     *     <Event>string</Event>
     *     ...
     *     <Filter>
     *       <S3Key>
     *         <FilterRule>
     *           <Name>string</Name>
     *           <Value>string</Value>
     *        </FilterRule>
     *        ...
     *       </S3Key>
     *     </Filter>
     *     <Id>string</Id>
     *     <CloudFunction>string</CloudFunction>
     *    </CloudFunctionConfiguration>
     *    ...
     *    <EventBridgeConfiguration>
     *    </EventBridgeConfiguration>
     * </NotificationConfiguration>
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PutBucketNotificationConfigurationRequest {

        /**
         * @brief AWS region
         */
        std::string region;

        /**
         * @brief Bucket
         */
        std::string bucket;

        /**
         * @brief SNS topic configurations
         */
        std::vector<TopicConfiguration> topicConfigurations;

        /**
         * @brief SQS queue configurations
         */
        std::vector<QueueConfiguration> queueConfigurations;

        /**
         * @brief Lambda function configurations
         */
        std::vector<LambdaConfiguration> lambdaConfigurations;

        /**
          * @brief Convert from XML representation
          *
          * @param xmlString XML string
          */
        void FromXml(const std::string &xmlString);

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
        friend std::ostream &operator<<(std::ostream &os, const PutBucketNotificationConfigurationRequest &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_REQUEST_H
