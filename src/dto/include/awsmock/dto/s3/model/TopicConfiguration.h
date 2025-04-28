//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_TOPIC_CONFIGURATION_H
#define AWSMOCK_DTO_S3_TOPIC_CONFIGURATION_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/s3/model/FilterRule.h>
#include <awsmock/dto/s3/model/NotificationEvent.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief S3 notification configuration for an SNS topic.
     *
     * <p>
     * This configures the S3 module to send a message to the supplied SNS topic. Depending on the event type and the
     * filter rules, the event is executed and a message is sent to the provided SNS queue.
     * </p>
     *
     * Example:
     * @code{.xml}
     *   <TopicConfiguration>
     *     <Event>string</Event>
     *     ...
     *     <Filter>
     *       <S3Key>
     *         <FilterRule>
     *           <Name>string</Name>
     *           <Value></Value>
     *         </FilterRule>
     *         ...
     *      </S3Key>
     *     </Filter>
     *     <Id>string</Id>
     *     <Topic>string</Topic>
     *   </TopicConfiguration>
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct TopicConfiguration : Common::BaseDto<TopicConfiguration> {

        /**
         * ID, optional, if empty a random ID will be generated
         */
        std::string id;

        /**
         * Topic ARN
         */
        std::string topicArn;

        /**
         * S3 filter rules
         */
        std::vector<FilterRule> filterRules;

        /**
         * Notification events
         */
        std::vector<NotificationEventType> events;

        /**
         * @brief Convert from a JSON object
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document);

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Convert from an XML string
         *
         * @param pt boost a property tree
         */
        void FromXml(const boost::property_tree::ptree &pt);

        std::string ToJson() const override {
            return ToJson2();
        }

      private:

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, TopicConfiguration const &obj) {
            jv = {
                    {"id", obj.id},
                    {"topicArn", obj.topicArn},
                    {"filterRules", boost::json::value_from(obj.filterRules)},
                    {"events", boost::json::value_from(obj.events)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_TOPIC_CONFIGURATION_H
