//
// Created by vogje01 on 31/05/2023.
//

#ifndef AWSMOCK_DTO_MODULE_LIST_NAMES_RESPONSE_H
#define AWSMOCK_DTO_MODULE_LIST_NAMES_RESPONSE_H

// C++ Standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Dto::Module {

    struct ListModuleNamesResponse {

        /**
         * Module names list
         */
        std::vector<std::string> moduleNames;

        /**
         * @brief Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * @brief Convert from a JSON string
         *
         * @param jsonString JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const ListModuleNamesResponse &r);
    };

}// namespace AwsMock::Dto::Module

#endif// AWSMOCK_DTO_MODULE_LIST_NAMES_RESPONSE_H
