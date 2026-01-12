//
// Created by vogje01 on 13/10/2023.
//

#ifndef AWSMOCK_CORE_DTO_PUT_BUCKET_LIFECYCLE_CONFIGURATION_REQUEST_H
#define AWSMOCK_CORE_DTO_PUT_BUCKET_LIFECYCLE_CONFIGURATION_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock include
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/s3/model/LifecycleRule.h>

namespace AwsMock::Dto::S3 {

    struct PutBucketLifecycleConfigurationRequest final : Common::BaseCounter<PutBucketLifecycleConfigurationRequest> {

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * S3 lifecycle rules
         */
        std::vector<LifecycleRule> rules;

        /**
         * @brief Convert from XML representation
         *
         * @param xmlString XML string
         */
        void FromXml(const std::string &xmlString) {

            try {

                boost::property_tree::ptree pt;
                Core::XmlUtils::ReadXml(xmlString, &pt);

                // Iterate over all children of the root
                for (const boost::property_tree::ptree &root = pt.get_child("LifecycleConfiguration"); const auto &[fst, snd]: root) {

                    // We only care about nodes named "Rule"
                    if (fst == "Rule") {
                        LifecycleRule rule;
                        rule.FromXml(snd);
                        rules.push_back(rule);
                    }
                }

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend PutBucketLifecycleConfigurationRequest tag_invoke(boost::json::value_to_tag<PutBucketLifecycleConfigurationRequest>, boost::json::value const &v) {
            PutBucketLifecycleConfigurationRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.rules = boost::json::value_to<std::vector<LifecycleRule>>(v.at("rules"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutBucketLifecycleConfigurationRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"rules", boost::json::value_from(obj.rules)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_PUT_BUCKET_LIFECYCLE_CONFIGURATION_REQUEST_H
