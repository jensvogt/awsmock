//
// Created by vogje01 on 5/31/24.
//

#ifndef AWSMOCK_DTO_COGNITO_GROUP_H
#define AWSMOCK_DTO_COGNITO_GROUP_H


// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/LogStream.h>

namespace AwsMock::Dto::Cognito {

    using std::chrono::system_clock;

    /**
     * @brief Cognito user group
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Group {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Group name
         */
        std::string groupName;

        /**
         * User pool Id
         */
        std::string userPoolId;

        /**
         * Description
         */
        std::string description;

        /**
         * Role ARN
         */
        std::string roleArn;

        /**
         * Precedence
         */
        int precedence;

        /**
         * Created
         */
        system_clock::time_point created = system_clock::now();

        /**
         * Last modified
         */
        system_clock::time_point modified = system_clock::now();

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

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
        friend std::ostream &operator<<(std::ostream &os, const Group &r);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_GROUP_H
