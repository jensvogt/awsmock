//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_LAMBDA_CONFIGURATION_H
#define AWSMOCK_DTO_S3_LAMBDA_CONFIGURATION_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
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
    struct LambdaConfiguration final : Common::BaseCounter<LambdaConfiguration> {

        /**
         * ID, optional, if empty, a random ID will be generated
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
         * @brief Convert from an XML string
         *
         * @param pt boost a property tree
         */
        void FromXml(const boost::property_tree::ptree &pt) {

            if (pt.get_optional<std::string>("Id")) {
                id = pt.get<std::string>("Id");
            }
            if (pt.get_optional<std::string>("CloudFunction")) {
                lambdaArn = pt.get<std::string>("CloudFunction");
            }
            if (pt.get_optional<std::string>("Filter")) {
                FilterRule filter;
                filter.FromXml(pt.get_child("Filter"));
                filterRules.emplace_back(filter);
            }
            if (pt.get_optional<std::string>("Event")) {
                events.emplace_back(EventTypeFromString(pt.get<std::string>("Event")));
            }
        }

      private:

        friend LambdaConfiguration tag_invoke(boost::json::value_to_tag<LambdaConfiguration>, boost::json::value const &v) {
            LambdaConfiguration r;
            r.id = Core::Json::GetStringValue(v, "id");
            r.lambdaArn = Core::Json::GetStringValue(v, "lambdaArn");
            r.filterRules = boost::json::value_to<std::vector<FilterRule>>(v.at("filterRules"));
            r.events = boost::json::value_to<std::vector<NotificationEventType>>(v.at("events"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, LambdaConfiguration const &obj) {
            jv = {
                    {"id", obj.id},
                    {"lambdaArn", obj.lambdaArn},
                    {"filterRules", boost::json::value_from(obj.filterRules)},
                    {"events", boost::json::value_from(obj.events)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_TOPIC_CONFIGURATION_H
