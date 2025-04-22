//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_S3_GET_OBJECT_REQUEST_H
#define AWSMOCK_DTO_S3_GET_OBJECT_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "awsmock/dto/common/BaseDto.h"


#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Get object request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetObjectRequest : Common::BaseDto<GetObjectRequest> {

        /**
         * AWS region name
         */
        std::string region;

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::string key;

        /**
         * Version ID
         */
        std::string versionId;

        /**
         * Range minimum
         */
        long min;

        /**
         * Range maximum
         */
        long max;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_GET_OBJECT_REQUEST_H
