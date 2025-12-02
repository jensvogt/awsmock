//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_S3_COMPLETE_MULTIPART_UPLOAD_REQUEST_H
#define AWSMOCK_CORE_DTO_S3_COMPLETE_MULTIPART_UPLOAD_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Complete multipart upload
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CompleteMultipartUploadRequest final : Common::BaseCounter<CompleteMultipartUploadRequest> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Bucket
         */
        std::string key;

        /**
         * Upload ID
         */
        std::string uploadId;

        /**
         * Checksum CRC32
         */
        std::string checksumCrc32;

        /**
         * Checksum CRC32C
         */
        std::string checksumCrc32c;

        /**
         * Checksum SAH1
         */
        std::string checksumSha1;

        /**
         * Checksum SAH256
         */
        std::string checksumSha256;

        /**
         * ETag
         */
        std::string eTag;

        /**
         * Part number
         */
        long partNumber{};

        /**
         * Content type
         */
        std::string contentType;

        /**
         * @brief Convert from XML representation
         *
         * @param xmlString XML string
         */
        void FromXml(const std::string &xmlString) {

            try {

                boost::property_tree::ptree pt;
                read_xml(xmlString, pt);
                checksumCrc32 = pt.get<std::string>("CompleteMultipartUpload.Part.ChecksumCRC32");
                checksumCrc32c = pt.get<std::string>("CompleteMultipartUpload.Part.ChecksumCRC32C");
                checksumSha1 = pt.get<std::string>("CompleteMultipartUpload.Part.ChecksumSHA1");
                checksumSha256 = pt.get<std::string>("CompleteMultipartUpload.Part.ChecksumSHA256");
                eTag = pt.get<std::string>("CompleteMultipartUpload.Part.ETag");
                partNumber = pt.get<int>("CompleteMultipartUpload.Part.PartNumber");

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Convert to an XML string
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            try {

                boost::property_tree::ptree root;
                root.add("CompleteMultipartUpload.Part.ETag", eTag);
                root.add("CompleteMultipartUpload.Part.ChecksumCRC32", checksumCrc32);
                root.add("CompleteMultipartUpload.Part.ChecksumCRC32C", checksumCrc32c);
                root.add("CompleteMultipartUpload.Part.ChecksumSHA1", checksumSha1);
                root.add("CompleteMultipartUpload.Part.ChecksumSHA256", checksumSha256);
                root.add("CompleteMultipartUpload.Part.PartNumber", partNumber);
                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend CompleteMultipartUploadRequest tag_invoke(boost::json::value_to_tag<CompleteMultipartUploadRequest>, boost::json::value const &v) {
            CompleteMultipartUploadRequest r;
            r.bucket = Core::Json::GetStringValue(v, "Bucket");
            r.key = Core::Json::GetStringValue(v, "Key");
            r.uploadId = Core::Json::GetStringValue(v, "UploadId");
            r.checksumCrc32 = Core::Json::GetStringValue(v, "ChecksumCrc32");
            r.checksumCrc32c = Core::Json::GetStringValue(v, "ChecksumCrc32c");
            r.checksumSha1 = Core::Json::GetStringValue(v, "ChecksumSha1");
            r.checksumSha256 = Core::Json::GetStringValue(v, "ChecksumSha256");
            r.eTag = Core::Json::GetStringValue(v, "ETag");
            r.partNumber = Core::Json::GetLongValue(v, "PartNunmber");
            r.contentType = Core::Json::GetStringValue(v, "ContentType");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CompleteMultipartUploadRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Bucket", obj.bucket},
                    {"Key", obj.key},
                    {"UploadId", obj.uploadId},
                    {"ChecksumCrc32", obj.checksumCrc32},
                    {"ChecksumCrc32c", obj.checksumCrc32c},
                    {"ChecksumSha1", obj.checksumSha1},
                    {"ChecksumSha256", obj.checksumSha256},
                    {"ETag", obj.eTag},
                    {"PartNumber", obj.partNumber},
                    {"ContentType", obj.contentType},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_S3_COMPLETE_MULTIPART_UPLOAD_REQUEST_H
