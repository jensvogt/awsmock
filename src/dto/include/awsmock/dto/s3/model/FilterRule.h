//
// Created by vogje01 on 4/28/24.
//

#ifndef AWSMOCK_DTO_S3_FILTER_RULE_H
#define AWSMOCK_DTO_S3_FILTER_RULE_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief S3 name type for the filter rules
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    enum class NameType {
        prefix,
        suffix
    };

    static std::map<NameType, std::string> NameTypeNames{
            {NameType::prefix, "prefix"},
            {NameType::suffix, "suffix"}};

    [[maybe_unused]] static std::string NameTypeToString(const NameType &nameType) {
        return NameTypeNames[nameType];
    }

    [[maybe_unused]] static NameType NameTypeFromString(const std::string &nameType) {
        for (auto &[fst, snd]: NameTypeNames) {
            if (snd == nameType) {
                return fst;
            }
        }
        return NameType::prefix;
    }

    BOOST_DESCRIBE_ENUM(NameType,
                        prefix,
                        prefix);

    /**
     * @brief Filter rule for the S3 bucket notification to SQS queues
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct FilterRule final : Common::BaseCounter<FilterRule> {

        /**
         * Name
         */
        NameType name = NameType::prefix;

        /**
         * Value
         */
        std::string filterValue;

        /**
         * @brief Convert from an XML string
         *
         * @param pt boost a property tree
         */
        void FromXml(const boost::property_tree::ptree &pt) {
            if (pt.get_child_optional("S3Key")) {
                if (const boost::property_tree::ptree s3KeyNode = pt.get_child("S3Key"); s3KeyNode.get_child_optional("FilterRule")) {
                    const boost::property_tree::ptree filterRuleNode = s3KeyNode.get_child("FilterRule");
                    if (filterRuleNode.get_optional<std::string>("Name")) {
                        name = NameTypeFromString(filterRuleNode.get<std::string>("Name"));
                    }
                    if (filterRuleNode.get_optional<std::string>("Value")) {
                        filterValue = filterRuleNode.get<std::string>("Value");
                    }
                }
            }
        }

      private:

        friend FilterRule tag_invoke(boost::json::value_to_tag<FilterRule>, boost::json::value const &v) {
            FilterRule r;
            r.name = NameTypeFromString(Core::Json::GetStringValue(v, "name"));
            r.filterValue = Core::Json::GetStringValue(v, "filterValue");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, FilterRule const &obj) {
            jv = {
                    {"name", NameTypeToString(obj.name)},
                    {"filterValue", obj.filterValue},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_FILTER_RULE_H
