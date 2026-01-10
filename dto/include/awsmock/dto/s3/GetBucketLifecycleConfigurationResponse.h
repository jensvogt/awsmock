//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_BUCKET_LIFECYCLE_CONFIGURATION_RESPONSE_H
#define AWSMOCK_DTO_S3_GET_BUCKET_LIFECYCLE_CONFIGURATION_RESPONSE_H

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/s3/model/LifecycleRule.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief S3 Lifecycle configuration
     *
     * Example:
     * @code{.xml}
     * <LifecycleConfiguration>
     *   <Rule>
     *     <AbortIncompleteMultipartUpload>
     *       <DaysAfterInitiation>integer</DaysAfterInitiation>
     *     </AbortIncompleteMultipartUpload>
     *     <Expiration>
     *       <Date>timestamp</Date>
     *       <Days>integer</Days>
     *       <ExpiredObjectDeleteMarker>boolean</ExpiredObjectDeleteMarker>
     *     </Expiration>
     *     <Filter>
     *       <And>
     *         <ObjectSizeGreaterThan>long</ObjectSizeGreaterThan>
     *         <ObjectSizeLessThan>long</ObjectSizeLessThan>
     *         <Prefix>string</Prefix>
     *         <Tag>
     *            <Key>string</Key>
     *            <Value>string</Value>
     *         </Tag>
     *         ...
     *      </And>
     *      <ObjectSizeGreaterThan>long</ObjectSizeGreaterThan>
     *      <ObjectSizeLessThan>long</ObjectSizeLessThan>
     *      <Prefix>string</Prefix>
     *      <Tag>
     *         <Key>string</Key>
     *         <Value>string</Value>
     *      </Tag>
     *   </Filter>
     *   <ID>string</ID>
     *   <NoncurrentVersionExpiration>
     *      <NewerNoncurrentVersions>integer</NewerNoncurrentVersions>
     *      <NoncurrentDays>integer</NoncurrentDays>
     *   </NoncurrentVersionExpiration>
     *   <NoncurrentVersionTransition>
     *      <NewerNoncurrentVersions>integer</NewerNoncurrentVersions>
     *      <NoncurrentDays>integer</NoncurrentDays>
     *      <StorageClass>string</StorageClass>
     *   </NoncurrentVersionTransition>
     *   ...
     *   <Prefix>string</Prefix>
     *   <Status>string</Status>
     *   <Transition>
     *     <Date>timestamp</Date>
     *     <Days>integer</Days>
     *      <StorageClass>string</StorageClass>
     *   </Transition>
     *   ...
     *  </Rule>
     *  ...
     * </LifecycleConfiguration>
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetBucketLifecycleConfigurationResponse final : Common::BaseCounter<GetBucketLifecycleConfigurationResponse> {

        /**
         * Topic configurations
         */
        std::vector<LifecycleRule> rules;

        /**
          * @brief Convert from XML representation
          *
          * @return xmlString XML string
          */
        [[nodiscard]] std::string ToXml() const {

            boost::property_tree::ptree lifecycleConfigration;

            // Rules
            for (const auto &rule: rules) {
                lifecycleConfigration.add_child("Rule", rule.ToXml());
            }

            boost::property_tree::ptree root;
            root.add_child("LifecycleConfiguration", lifecycleConfigration);

            return Core::XmlUtils::ToXmlString(root);
        }

      private:

        friend GetBucketLifecycleConfigurationResponse tag_invoke(boost::json::value_to_tag<GetBucketLifecycleConfigurationResponse>, boost::json::value const &v) {
            GetBucketLifecycleConfigurationResponse r;
            if (Core::Json::AttributeExists(v, "LifecycleRule")) {
                r.rules = boost::json::value_to<std::vector<LifecycleRule>>(v.at("LifecycleRule"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, GetBucketLifecycleConfigurationResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"LifecycleRule", boost::json::value_from(obj.rules)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_GET_BUCKET_LIFECYCLE_CONFIGURATION_RESPONSE_H
