//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_COGNITO_USER_ATTRIBUTE_H
#define AWSMOCK_DTO_COGNITO_USER_ATTRIBUTE_H

// C++ standard includes
#include <sstream>
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/entity/cognito/User.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief User attribute
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UserAttribute {

        /**
         * User attribute name
         */
        std::string name;

        /**
         * MessageAttribute value
         */
        std::string value;

        /**
         * @brief Converts the JSON string to a DTO
         *
         * @param jsonObject JSON object
         */
        void FromJsonObject(const Poco::JSON::Object::Ptr &jsonObject);

        /**
         * @brief Convert to a JSON string.
         *
         * @return user pools json string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Convert to a JSON string.
         *
         * @return user pools json string
         */
        Poco::JSON::Object ToJsonObject() const;

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const UserAttribute &r);
    };

    typedef std::vector<UserAttribute> UserAttributeList;

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_USER_ATTRIBUTE_H
