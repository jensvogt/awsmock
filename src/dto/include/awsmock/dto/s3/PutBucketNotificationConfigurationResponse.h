//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_RESPONSE_H
#define AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_RESPONSE_H

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
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
    struct PutBucketNotificationConfigurationResponse final : Common::BaseCounter<PutBucketNotificationConfigurationResponse> {

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
        [[nodiscard]] std::string ToXml() const {

            boost::property_tree::ptree root;

            // Queue notification configurations
            for (const auto &queueConfiguration: queueConfigurations) {

                boost::property_tree::ptree xmlQueueNotification;
                xmlQueueNotification.add("Id", queueConfiguration.id);
                xmlQueueNotification.add("QueueArn", queueConfiguration.queueArn);

                // Events
                if (!queueConfiguration.events.empty()) {
                    boost::property_tree::ptree xmlEventArray;
                    for (const auto &event: queueConfiguration.events) {
                        std::string strEvent = EventTypeToString(event);
                        xmlEventArray.push_back(boost::property_tree::basic_ptree<std::string, std::string>::value_type(std::make_pair("", strEvent)));
                    }
                    xmlQueueNotification.add_child("Events", xmlEventArray);
                }
                root.add_child("NotificationConfiguration.QueueConfiguration", xmlQueueNotification);
            }

            // Topic notification configurations
            for (const auto &topicConfiguration: topicConfigurations) {

                boost::property_tree::ptree xmlTopicNotification;
                xmlTopicNotification.add("Id", topicConfiguration.id);
                xmlTopicNotification.add("TopicArn", topicConfiguration.topicArn);

                // Events
                if (!topicConfiguration.events.empty()) {
                    boost::property_tree::ptree xmlEventArray;
                    for (const auto &event: topicConfiguration.events) {
                        std::string strEvent = EventTypeToString(event);
                        xmlEventArray.push_back(boost::property_tree::basic_ptree<std::string, std::string>::value_type(std::make_pair("", strEvent)));
                    }
                    xmlTopicNotification.add_child("Events", xmlEventArray);
                }
                root.add_child("NotificationConfiguration.TopicConfiguration", xmlTopicNotification);
            }

            // Lambda notification configurations
            for (const auto &lambdaConfiguration: lambdaConfigurations) {

                boost::property_tree::ptree xmlLambdaNotification;
                xmlLambdaNotification.add("Id", lambdaConfiguration.id);
                xmlLambdaNotification.add("LambdaArn", lambdaConfiguration.lambdaArn);

                // Events
                if (!lambdaConfiguration.events.empty()) {
                    boost::property_tree::ptree xmlEventArray;
                    for (const auto &event: lambdaConfiguration.events) {
                        std::string strEvent = EventTypeToString(event);
                        xmlEventArray.push_back(boost::property_tree::basic_ptree<std::string, std::string>::value_type(std::make_pair("", strEvent)));
                    }
                    xmlLambdaNotification.add_child("Events", xmlEventArray);
                }
                root.add_child("NotificationConfiguration.LambdaConfiguration", xmlLambdaNotification);
            }
            return Core::XmlUtils::ToXmlString(root);
        }

      private:

        friend PutBucketNotificationConfigurationResponse tag_invoke(boost::json::value_to_tag<PutBucketNotificationConfigurationResponse>, boost::json::value const &v) {
            PutBucketNotificationConfigurationResponse r;
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

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutBucketNotificationConfigurationResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"topicConfigurations", boost::json::value_from(obj.topicConfigurations)},
                    {"queueConfigurations", boost::json::value_from(obj.queueConfigurations)},
                    {"lambdaConfigurations", boost::json::value_from(obj.lambdaConfigurations)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_PUT_BUCKET_NOTIFICATION_CONFIGURATION_RESPONSE_H
