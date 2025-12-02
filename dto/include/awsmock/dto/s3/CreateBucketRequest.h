//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_S3_CREATE_BUCKET_REQUEST_H
#define AWSMOCK_CORE_DTO_S3_CREATE_BUCKET_REQUEST_H

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
    struct CreateBucketRequest final : Common::BaseCounter<CreateBucketRequest> {

        /**
         * Bucket name
         */
        std::string name{};

        /**
         * Bucket owner
         */
        std::string owner{};

        /**
         * @brief Convert from XML representation
         *
         * @param xmlString XML string
         */
        void FromXml(const std::string &xmlString) {

            try {

                boost::property_tree::ptree pt;
                read_xml(xmlString, pt);
                region = pt.get<std::string>("CreateBucketConfiguration.LocationConstraint");

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend CreateBucketRequest tag_invoke(boost::json::value_to_tag<CreateBucketRequest>, boost::json::value const &v) {
            CreateBucketRequest r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.owner = Core::Json::GetStringValue(v, "Owner");
            return r;
        }
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CreateBucketRequest const &obj) {
            jv = {
                    {"Name", obj.name},
                    {"Owner", obj.owner},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_S3_CREATE_BUCKET_REQUEST_H
