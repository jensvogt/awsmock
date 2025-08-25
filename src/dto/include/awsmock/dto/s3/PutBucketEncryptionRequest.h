//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_DTO_S3_PUT_BUCKET_ENCRYPTION_REQUEST_H
#define AWSMOCK_DTO_S3_PUT_BUCKET_ENCRYPTION_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief S3 encryption configuration
     *
     * This action uses the encryption subresource to configure default encryption and Amazon S3 Bucket Keys for an existing bucket.
     * By default, all buckets have a default encryption configuration that uses server-side encryption with Amazon S3 managed
     * keys (SSE-S3). You can optionally configure default encryption for a bucket by using server-side encryption with AWS Key
     * Management Service (AWS KMS) keys (SSE-KMS) or dual-layer server-side encryption with AWS KMS keys (DSSE-KMS). If you
     * specify default encryption by using SSE-KMS, you can also configure Amazon S3 Bucket Keys. If you use PutBucketEncryption
     * to set your default bucket encryption to SSE-KMS, you should verify that your KMS key ID is correct. Amazon S3 does not
     * validate the KMS key ID provided in PutBucketEncryption requests.
     *
     * Example:
     * @code{.xml}
     * <ServerSideEncryptionConfiguration xmlns="http://s3.amazonaws.com/doc/2006-03-01/">
     *   <Rule>
     *     <ApplyServerSideEncryptionByDefault>
     *       <SSEAlgorithm>aws:kms:dsse</SSEAlgorithm>
     *       <KMSKeyID>arn:aws:kms:us-east-1:1234/5678example</KMSKeyID>
     *     </ApplyServerSideEncryptionByDefault>
     *   </Rule>
     * </ServerSideEncryptionConfiguration>
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct PutBucketEncryptionRequest final : Common::BaseCounter<PutBucketEncryptionRequest> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Server site encryption algorithm
         */
        std::string sseAlgorithm;

        /**
         * KMS key ID
         */
        std::string kmsKeyId;

        /**
          * Convert from XML representation
          *
          * @param xmlString XML string
          */
        void FromXml(const std::string &xmlString) {

            try {

                boost::property_tree::ptree pt;
                Core::XmlUtils::ReadXml(xmlString, &pt);

                sseAlgorithm = pt.get<std::string>("ServerSideEncryptionConfiguration.Rule.ApplyServerSideEncryptionByDefault.SSEAlgorithm");
                kmsKeyId = pt.get<std::string>("ServerSideEncryptionConfiguration.Rule.ApplyServerSideEncryptionByDefault.KMSMasterKeyID");

            } catch (std::exception &e) {
                log_error << e.what();
                throw Core::JsonException(e.what());
            }
        }

      private:

        friend PutBucketEncryptionRequest tag_invoke(boost::json::value_to_tag<PutBucketEncryptionRequest>, boost::json::value const &v) {
            PutBucketEncryptionRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            r.sseAlgorithm = Core::Json::GetStringValue(v, "sseAlgorithm");
            r.kmsKeyId = Core::Json::GetStringValue(v, "kmsKeyId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, PutBucketEncryptionRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"sseAlgorithm", obj.sseAlgorithm},
                    {"kmsKeyId", obj.kmsKeyId},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_PUT_BUCKET_ENCRYPTION_REQUEST_H
