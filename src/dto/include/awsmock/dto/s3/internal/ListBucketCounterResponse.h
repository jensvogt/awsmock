//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_LIST_BUCKET_COUNTERS_RESPONSE_H
#define AWSMOCK_DTO_S3_LIST_BUCKET_COUNTERS_RESPONSE_H

// C++ standard includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/XmlUtils.h>
#include <awsmock/dto/s3/model/BucketCounter.h>

namespace AwsMock::Dto::S3 {

    struct ListBucketCounterResponse {

        /**
         * List of buckets
         */
        std::vector<BucketCounter> bucketCounters;

        /**
         * Total number of buckets
         */
        long total = 0;

        /**
         * @brief Convert to JSON representation
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ListBucketCounterResponse &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIST_ALL_BUCKET_RESPONSE_H
