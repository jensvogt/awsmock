//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_QUEUE_CONFIGURATION_H
#define AWSMOCK_DTO_S3_QUEUE_CONFIGURATION_H

// C++ standard includes
#include <string>
#include <vector>

// Boost includes
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <boost/mp11.hpp>
#include <boost/version.hpp>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/s3/model/FilterRule.h>
#include <awsmock/dto/s3/model/NotificationEvent.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief S3 notification configuration for a SQS queue.
     *
     * <p>
     * This configures the S3 module to send a message to the supplied SQS queue. Depending on the event type and the
     * filter rules, the event is executed and a message is send to the provided SQS queue.
     * </p>
     *
     * Example:
     * @code{.xml}
     *   <QueueConfiguration>
     *     <Event>string</Event>
     *     ...
     *     <Filter>
     *       <S3Key>
     *         <FilterRule>
     *           <Name>string</Name>
     *           <Value>string</Value>
     *         </FilterRule>
     *         ...
     *      </S3Key>
     *     </Filter>
     *     <Id>string</Id>
     *     <Queue>string</Queue>
     *   </QueueConfiguration>
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct QueueConfiguration {

        /**
         * ID, optional, if empty, a random ID will be generated
         */
        std::string id;

        /**
         * Queue ARN
         */
        std::string queueArn;

        /**
         * S3 filter rules
         */
        std::vector<FilterRule> filterRules;

        /**
         * Notification events
         */
        std::vector<NotificationEventType> events;

        /**
         * Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Convert from a JSON object
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document);

        /**
         * @brief Convert from an XML string
         *
         * @param pt boost a property tree
         */
        void FromXml(const boost::property_tree::ptree &pt);

      private:

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, QueueConfiguration const &obj) {
            jv = {
                    {"id", obj.id},
                    {"queueArn", obj.queueArn},
                    {"filterRules", boost::json::value_from(obj.filterRules)},
                    {"events", boost::json::value_from(obj.events)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_QUEUE_CONFIGURATION_H
