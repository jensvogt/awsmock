//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_COMPLETE_MULTIPART_UPLOAD_RESULT_H
#define AWSMOCK_DTO_S3_COMPLETE_MULTIPART_UPLOAD_RESULT_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Complete multipart upload
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CompleteMultipartUploadResult final : Common::BaseCounter<CompleteMultipartUploadResult> {

        /**
         * AWS location
         */
        std::string location;

        /**
         * Bucket name
         */
        std::string bucket;

        /**
         * Key name
         */
        std::string key;

        /**
         * ETag
         */
        std::string etag;

        /**
         * Checksum CRC32
         */
        std::string checksumCrc32;

        /**
         * Checksum CRC32C
         */
        std::string checksumCrc32c;

        /**
         * Checksum SHA1
         */
        std::string checksumSha1;

        /**
         * Checksum SHA256
         */
        std::string checksumSha256;

        /**
         * @brief Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            boost::property_tree::ptree root;
            root.add("CompleteMultipartUploadResult.Location", location);
            root.add("CompleteMultipartUploadResult.Bucket", bucket);
            root.add("CompleteMultipartUploadResult.Key", key);
            root.add("CompleteMultipartUploadResult.ETag", etag);
            root.add("CompleteMultipartUploadResult.ChecksumCRC32", checksumCrc32);
            root.add("CompleteMultipartUploadResult.ChecksumCRC32C", checksumCrc32c);
            root.add("CompleteMultipartUploadResult.ChecksumSHA1", checksumSha1);
            root.add("CompleteMultipartUploadResult.ChecksumSHA256", checksumSha256);

            return Core::XmlUtils::ToXmlString(root);
        }

      private:

        friend CompleteMultipartUploadResult tag_invoke(boost::json::value_to_tag<CompleteMultipartUploadResult>, boost::json::value const &v) {
            CompleteMultipartUploadResult r;
            r.bucket = Core::Json::GetStringValue(v, "Bucket");
            r.key = Core::Json::GetStringValue(v, "Key");
            r.location = Core::Json::GetStringValue(v, "UploadId");
            r.checksumCrc32 = Core::Json::GetStringValue(v, "ChecksumCrc32");
            r.checksumCrc32c = Core::Json::GetStringValue(v, "ChecksumCrc32c");
            r.checksumSha1 = Core::Json::GetStringValue(v, "ChecksumSha1");
            r.checksumSha256 = Core::Json::GetStringValue(v, "ChecksumSha256");
            r.contentType = Core::Json::GetStringValue(v, "ContentType");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CompleteMultipartUploadResult const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Bucket", obj.bucket},
                    {"Key", obj.key},
                    {"Location", obj.location},
                    {"ChecksumCrc32", obj.checksumCrc32},
                    {"ChecksumCrc32c", obj.checksumCrc32c},
                    {"ChecksumSha1", obj.checksumSha1},
                    {"ChecksumSha256", obj.checksumSha256},
                    {"ContentType", obj.contentType},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_COMPLETE_MULTIPART_UPLOAD_RESULT_H
