//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_RESPONSE_H
#define AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_RESPONSE_H

// C++ standard includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * @brief Upload part copy response
     *
     * Example:
     * @code{.json}
     * <CopyPartResult>
     *   <ETag>string</ETag>
     *   <LastModified>timestamp</LastModified>
     *   <ChecksumCRC32>string</ChecksumCRC32>
     *   <ChecksumCRC32C>string</ChecksumCRC32C>
     *   <ChecksumSHA1>string</ChecksumSHA1>
     *   <ChecksumSHA256>string</ChecksumSHA256>
     * </CopyPartResult>
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UploadPartCopyResponse final : Common::BaseCounter<UploadPartCopyResponse> {

        /**
         * ETag
         */
        std::string eTag;

        /**
         * Last modified timestamp
         */
        system_clock::time_point lastModified;

        /**
         * Checksum CRC32
         */
        std::string checksumCRC32 = {};

        /**
         * Checksum CRC32C
         */
        std::string checksumCRC32C = {};

        /**
         * Checksum SHA1
         */
        std::string checksumSHA1 = {};

        /**
         * Checksum SHA256
         */
        std::string checksumSHA256 = {};

        /**
         * Convert to an XML string
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            try {

                boost::property_tree::ptree root;
                root.add("CopyPartResult.ETag", eTag);
                root.add("CopyPartResult.ChecksumCRC32", checksumCRC32);
                root.add("CopyPartResult.ChecksumCRC32C", checksumCRC32C);
                root.add("CopyPartResult.ChecksumSHA1", checksumSHA1);
                root.add("CopyPartResult.ChecksumSHA256", checksumSHA256);
                root.add("CopyPartResult.LastModified", Core::DateTimeUtils::ToISO8601(lastModified));
                root.add("CopyPartResult.ETag", eTag);
                return Core::XmlUtils::ToXmlString(root);

            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend UploadPartCopyResponse tag_invoke(boost::json::value_to_tag<UploadPartCopyResponse>, boost::json::value const &v) {
            UploadPartCopyResponse r;
            r.eTag = Core::Json::GetStringValue(v, "eTag");
            r.lastModified = Core::Json::GetDatetimeValue(v, "lastModified");
            r.checksumCRC32 = Core::Json::GetStringValue(v, "checksumCRC32");
            r.checksumCRC32C = Core::Json::GetStringValue(v, "checksumCRC32C");
            r.checksumSHA1 = Core::Json::GetStringValue(v, "checksumSHA1");
            r.checksumSHA256 = Core::Json::GetStringValue(v, "checksumSHA256");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, UploadPartCopyResponse const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"eTag", obj.eTag},
                    {"lastModified", Core::DateTimeUtils::ToISO8601(obj.lastModified)},
                    {"checksumCRC32", obj.checksumCRC32},
                    {"checksumCRC32C", obj.checksumCRC32C},
                    {"checksumSHA1", obj.checksumSHA1},
                    {"checksumSHA256", obj.checksumSHA256},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_UPLOAD_PART_COPY_RESPONSE_H
