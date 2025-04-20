//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_LAMBDA_CONFIGURATION_H
#define AWSMOCK_DTO_S3_LAMBDA_CONFIGURATION_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/s3/model/FilterRule.h>
#include <awsmock/dto/s3/model/NotificationEvent.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief S3 notification configuration for a Lambda function.
     *
     * <p>
     * This configures the S3 module to send a message to the supplied Lambda function. Depending on the event type
     * and the filter rules, the event is executed and a lambda function is called.
     * </p>
     *
     * Example:
     * @code{.xml}
     *   <CloudFunctionConfiguration>
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
     *     <CloudFunction>string</CloudFunction>
     *   </CloudFunctionConfiguration>
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LambdaConfiguration {

        /**
         * ID, optional, if empty a random ID will be generated
         */
        std::string id;

        /**
         * Lambda ARN
         */
        std::string lambdaArn;

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
         * @param pt boost property tree
         */
        void FromXml(const boost::property_tree::ptree &pt);

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
        friend std::ostream &operator<<(std::ostream &os, const LambdaConfiguration &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_TOPIC_CONFIGURATION_H
