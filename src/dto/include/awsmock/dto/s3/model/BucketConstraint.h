//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_BUCKET_CONSTRAINT_H
#define AWSMOCK_DTO_S3_BUCKET_CONSTRAINT_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief AWS S3 bucket constraints.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct BucketConstraint final : Common::BaseCounter<BucketConstraint> {

        /**
         * Bucket location
         */
        std::string location;

        /**
         * @brief Convert to XML representation
         *
         * Example:
         * @code{.xml}
         * <CreateBucketConfiguration xmlns="http://s3.amazonaws.com/doc/2006-03-01/">
         *     <LocationConstraint>eu-central-1</LocationConstraint>
         * </CreateBucketConfiguration>
         * @endcode
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            boost::property_tree::ptree root;
            root.add("CreateBucketConfiguration.LocationConstraint", location);
            return Core::XmlUtils::ToXmlString(root);
        }

      private:

        friend BucketConstraint tag_invoke(boost::json::value_to_tag<BucketConstraint>, boost::json::value const &v) {
            BucketConstraint r;
            r.location = Core::Json::GetStringValue(v, "Location");
            return r;
        }
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, BucketConstraint const &obj) {
            jv = {
                    {"Location", obj.location},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_BUCKET_CONSTRAINT_H
