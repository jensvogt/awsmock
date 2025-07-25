//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_CODE_H
#define AWSMOCK_DTO_LAMBDA_CODE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Lambda {

    /**
     * @brief Lambda code DTO
     *
     * Example:
     * @code{.json}
     * "Code": {
     *   "ImageUri": "string",
     *   "S3Bucket": "string",
     *   "S3Key": "string",
     *   "S3ObjectVersion": "string",
     *   "ZipFile": blob
     * },
     * @endcode
     *
     */
    struct Code final : Common::BaseCounter<Code> {

        /**
         * URI of a container image in the Amazon ECR registry.
         */
        std::string imageUri;

        /**
         * An Amazon S3 bucket in the same AWS Region as your function. The bucket can be in a different AWS account.
         */
        std::string s3Bucket;

        /**
         * The Amazon S3 key of the deployment package.
         */
        std::string s3Key;

        /**
         * For versioned objects, the version of the deployment package objects to use.
         */
        std::string s3ObjectVersion;

        /**
         * Zipped function code.
         *
         * @par
         * The base64-encoded contents of the deployment package. AWS SDK and AWS CLI clients handle the encoding for you.
         */
        std::string zipFile{};

        /**
         * Repository
         */
        std::string repositoryType;

        /**
         * Resolved image URI
         */
        std::string resolvedImageUri;

        /**
         * @brief Convert from a JSON object.
         *
         * @param document json object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                s3Bucket = Core::Bson::BsonUtils::GetStringValue(document, "S3Bucket");
                s3Key = Core::Bson::BsonUtils::GetStringValue(document, "S3Key");
                s3ObjectVersion = Core::Bson::BsonUtils::GetStringValue(document, "S3ObjectVersion");
                imageUri = Core::Bson::BsonUtils::GetStringValue(document, "ImageUri");
                zipFile = Core::Bson::BsonUtils::GetStringValue(document, "ZipFile");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Creates a JSON string from the object.
         *
         * @return JSON string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "ZipFile", zipFile);
                Core::Bson::BsonUtils::SetStringValue(document, "S3Bucket", s3Bucket);
                Core::Bson::BsonUtils::SetStringValue(document, "S3Key", s3Key);
                Core::Bson::BsonUtils::SetStringValue(document, "S3ObjectVersion", s3ObjectVersion);
                Core::Bson::BsonUtils::SetStringValue(document, "ImageUri", imageUri);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend Code tag_invoke(boost::json::value_to_tag<Code>, boost::json::value const &v) {
            Code r;
            r.imageUri = Core::Json::GetStringValue(v, "ImageUri");
            r.s3Bucket = Core::Json::GetStringValue(v, "S3Bucket");
            r.s3Key = Core::Json::GetStringValue(v, "S3Key");
            r.s3ObjectVersion = Core::Json::GetStringValue(v, "S3ObjectVersion");
            r.zipFile = Core::Json::GetStringValue(v, "ZipFile");
            r.repositoryType = Core::Json::GetStringValue(v, "RepositoryType");
            r.resolvedImageUri = Core::Json::GetStringValue(v, "ResolvedImageUri");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Code const &obj) {
            jv = {
                    {"ImageUri", obj.imageUri},
                    {"S3Bucket", obj.s3Bucket},
                    {"S3Key", obj.s3Key},
                    {"S3ObjectVersion", obj.s3ObjectVersion},
                    {"ZipFile", obj.zipFile},
                    {"RepositoryType", obj.repositoryType},
                    {"ResolvedImageUri", obj.resolvedImageUri},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_CODE_H
