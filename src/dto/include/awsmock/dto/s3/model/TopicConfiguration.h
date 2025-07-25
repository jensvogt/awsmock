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
#include <awsmock/core/logging/LogStream.h>
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
    struct TopicConfiguration : Common::BaseCounter<TopicConfiguration> {

        /**
         * ID, optional, if empty, a random ID will be generated
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

      private:

        friend TopicConfiguration tag_invoke(boost::json::value_to_tag<TopicConfiguration>, boost::json::value const &v) {
            TopicConfiguration r;
            r.id = v.at("id").as_string();
            r.topicArn = v.at("topicArn").as_string();

            // Filter rules
            for (auto filterRules = v.at("filterRules").as_array(); const auto &filterRule: filterRules) {
                FilterRule filterRuleDto;
                filterRuleDto.filterValue = filterRule.at("filterValue").as_string();
                filterRuleDto.name = NameTypeFromString(filterRule.at("name").as_string().data());
                r.filterRules.push_back(filterRuleDto);
            }

            // Events
            for (auto events = v.at("events").as_array(); const auto &event: events) {
                r.events.push_back(EventTypeFromString(event.as_string().data()));
            }
            return r;
        }

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
