//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_BUCKET_RESPONSE_H
#define AWSMOCK_DTO_S3_LIST_BUCKET_RESPONSE_H

// C++ Standard includes
#include <sstream>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/s3/model/Content.h>
#include <awsmock/dto/s3/model/Owner.h>
#include <awsmock/entity/s3/Object.h>

namespace AwsMock::Dto::S3 {

    struct CommonPrefix {

        /**
         * Prefix
         */
        std::string _prefix;
    };

    struct ListBucketResponse {

        /**
         * Truncation flag
         */
        bool isTruncated = false;

        /**
         * Contents
         */
        std::vector<Content> contents;

        /**
         * Name
         */
        std::string name;

        /**
         * Prefix
         */
        std::string prefix;

        /**
         * Delimiter
         */
        std::string delimiter;

        /**
         * Maximal keys
         */
        int maxKeys = 1000;

        /**
         * Encoding type
         */
        std::string encodingType;

        /**
         * Key count
         */
        int keyCount = 0;

        /**
         * Continuation token
         */
        std::string continuationToken;

        /**
         * Next continuation token
         */
        std::string nextContinuationToken;

        /**
         * Start after
         */
        std::string startAfter;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Constructor
         *
         * @param bucket bucket
         * @param objectList object list
         */
        ListBucketResponse(const std::string &bucket, const Database::Entity::S3::ObjectList &objectList);

        /**
         * Convert to XML representation
         *
         * @return XML string
         */
        std::string ToXml();

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ListBucketResponse &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIST_BUCKET_RESPONSE_H
