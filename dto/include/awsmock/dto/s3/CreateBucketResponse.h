//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_CREATE_BUCKET_RESPONSE_H
#define AWSMOCK_DTO_S3_CREATE_BUCKET_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    struct CreateBucketResponse final : Common::BaseCounter<CreateBucketResponse> {

        /**
         * Bucket location
         */
        std::string location;

        /**
         * Bucket ARN
         */
        std::string arn;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            boost::property_tree::ptree root;
            root.add("CreateBucketResult.BucketArn", arn);
            root.add("CreateBucketResult.Location", location);
            return Core::XmlUtils::ToXmlString(root);
        }

      private:

        friend CreateBucketResponse tag_invoke(boost::json::value_to_tag<CreateBucketResponse>, boost::json::value const &v) {
            CreateBucketResponse r;
            r.location = Core::Json::GetStringValue(v, "Location");
            r.arn = Core::Json::GetStringValue(v, "BucketArn");
            return r;
        }
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateBucketResponse const &obj) {
            jv = {
                    {"Location", obj.location},
                    {"BucketArn", obj.arn},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_CREATE_BUCKET_RESPONSE_H
