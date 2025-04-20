//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_RESTORE_STATUS_H
#define AWSMOCK_RESTORE_STATUS_H

// C++ includes
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief S3 object restore status DTO
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct RestoreStatus {

        /**
         * Is restore in progress
         */
        bool isRestoreInProgress;

        /**
         * Expiration datetime
         */
        system_clock::time_point restoreExpiryDate;

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        view_or_value<view, value> ToDocument() const;
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_RESTORE_STATUS_H
