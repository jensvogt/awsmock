//
// Created by vogje01 on 02/06/2023.
//

#ifndef AWSMOCK_DTO_COMMON_SERVICES_H
#define AWSMOCK_DTO_COMMON_SERVICES_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/StringUtils.h>

namespace AwsMock::Dto::Common {

    /**
     * @brief Services
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Services {

        /**
         * @brief Service names
         */
        std::vector<std::string> serviceNames;

        /**
         * @brief Check existence of given service name
         *
         * @param service name of the service
         * @return true in case the service exists
         */
        [[nodiscard]] bool HasService(const std::string &service) const;

        /**
         * @brief Check existence of given service name
         *
         * @param service name of the service
         * @return true in case the service exists
         */
        [[nodiscard]] bool Contains(const std::string &service) const;

        /**
         * @brief JSON representation
         *
         * @return Infrastructure as JSON string
         */
        std::string ToJson() const;

        /**
         * @brief JSON representation
         *
         * @param jsonString HTTP request body
         */
        void FromJson(const std::string &jsonString);

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
        friend std::ostream &operator<<(std::ostream &os, const Services &i);
    };

}// namespace AwsMock::Dto::Common

#endif// AWSMOCK_DTO_COMMON_SERVICES_H
