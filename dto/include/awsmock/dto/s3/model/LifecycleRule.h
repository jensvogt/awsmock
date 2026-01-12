//
// Created by vogje01 on 4/28/24.
//

#ifndef AWSMOCK_DTO_S3_LIFECYCLE_RULE_H
#define AWSMOCK_DTO_S3_LIFECYCLE_RULE_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/BaseDto.h>
#include <awsmock/dto/s3/model/LifecycleStatus.h>
#include <awsmock/dto/s3/model/LifecycleTransition.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Lifecycle rule for the S3 bucket
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LifecycleRule final : Common::BaseCounter<LifecycleRule> {

        /**
         * @brief ID
         *
         * @par
         * Default is randoUUID
         */
        std::string id = Core::StringUtils::CreateRandomUuid();

        /**
         * @brief Object prefix
         */
        std::string prefix;

        /**
         * @brief Lifecycle status
         */
        LifecycleStatus status = LIFECYCLE_DISABLED;

        /**
         * @brief Lifecycle transitions
         */
        std::vector<LifecycleTransition> transitions;

        /**
         * @brief Convert from an XML string
         *
         * @param pt boost a property tree
         */
        void FromXml(const boost::property_tree::ptree &pt) {

            // General
            id = pt.get<std::string>("ID", Core::StringUtils::CreateRandomUuid());
            status = LifeCycleStatusFromString(pt.get<std::string>("Status", "Disabled"));
            prefix = pt.get<std::string>("Prefix", "");

            // Transitions
            for (const auto &[fst, snd]: pt) {
                if (fst == "Transition") {
                    LifecycleTransition transition;
                    transition.FromXml(snd);
                    transitions.push_back(transition);
                }
            }
        }

        /**
         * @brief Generate from an XML string
         *
         * @return pt boost a property tree
         */
        [[nodiscard]] boost::property_tree::ptree ToXml() const {

            // General
            boost::property_tree::ptree pt;
            pt.put("ID", id);
            pt.put("Prefix", prefix);
            pt.put("Status", LifeCycleStatusToString(status));

            // Transitions
            if (!transitions.empty()) {
                for (const auto &transition: transitions) {
                    pt.add_child("Transition", transition.ToXml());
                }
            }
            return pt;
        }

      private:

        friend LifecycleRule tag_invoke(boost::json::value_to_tag<LifecycleRule>, boost::json::value const &v) {
            LifecycleRule r;
            r.id = Core::Json::GetStringValue(v, "ID");
            r.prefix = Core::Json::GetStringValue(v, "Prefix");
            r.status = LifeCycleStatusFromString(Core::Json::GetStringValue(v, "Status"));
            if (Core::Json::AttributeExists(v, "Transitions")) {
                r.transitions = boost::json::value_to<std::vector<LifecycleTransition>>(v.at("Transitions"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, LifecycleRule const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"ID", obj.id},
                    {"Prefix", obj.prefix},
                    {"Status", LifeCycleStatusToString(obj.status)},
                    {"Transitions", boost::json::value_from(obj.transitions)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIFECYCLE_RULE_H
