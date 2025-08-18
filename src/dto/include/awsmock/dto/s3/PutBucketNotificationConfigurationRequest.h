//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_REQUEST_H
#define AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_REQUEST_H

// AwsMock includes
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
    struct PutBucketNotificationConfigurationRequest final : Common::BaseCounter<PutBucketNotificationConfigurationRequest> {

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
        void FromXml(const std::string &xmlString) {

            try {

                boost::property_tree::ptree pt;
                Core::XmlUtils::ReadXml(xmlString, &pt);

                boost::property_tree::ptree notificationTree = pt.get_child("NotificationConfiguration");

                // SQS queues
                if (notificationTree.find("QueueConfiguration") != notificationTree.not_found()) {
                    QueueConfiguration queueConfiguration;
                    queueConfiguration.FromXml(notificationTree.get_child("QueueConfiguration"));
                    queueConfigurations.emplace_back(queueConfiguration);
                }

                // SNS topics
                if (notificationTree.find("TopicConfiguration") != notificationTree.not_found()) {
                    TopicConfiguration topicConfiguration;
                    topicConfiguration.FromXml(pt.get_child("TopicConfiguration"));
                    topicConfigurations.emplace_back(topicConfiguration);
                }

                // Lambdas
                if (notificationTree.find("CloudFunctionConfiguration") != notificationTree.not_found()) {
                    LambdaConfiguration lambdaConfiguration;
                    lambdaConfiguration.FromXml(notificationTree.get_child("CloudFunctionConfiguration"));
                    lambdaConfigurations.emplace_back(lambdaConfiguration);
                }

            } catch (std::exception &e) {
                log_error << e.what();
                throw Core::JsonException(e.what());
            }
        }

      private:

        friend PutBucketNotificationConfigurationRequest tag_invoke(boost::json::value_to_tag<PutBucketNotificationConfigurationRequest>, boost::json::value const &v) {
            PutBucketNotificationConfigurationRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            if (Core::Json::AttributeExists(v, "topicConfigurations")) {
                r.topicConfigurations = boost::json::value_to<std::vector<TopicConfiguration>>(v.at("topicConfigurations"));
            }
            if (Core::Json::AttributeExists(v, "queueConfigurations")) {
                r.queueConfigurations = boost::json::value_to<std::vector<QueueConfiguration>>(v.at("queueConfigurations"));
            }
            if (Core::Json::AttributeExists(v, "lambdaConfigurations")) {
                r.lambdaConfigurations = boost::json::value_to<std::vector<LambdaConfiguration>>(v.at("lambdaConfigurations"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutBucketNotificationConfigurationRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"topicConfigurations", boost::json::value_from(obj.topicConfigurations)},
                    {"queueConfigurations", boost::json::value_from(obj.queueConfigurations)},
                    {"lambdaConfigurations", boost::json::value_from(obj.lambdaConfigurations)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_REQUEST_H
