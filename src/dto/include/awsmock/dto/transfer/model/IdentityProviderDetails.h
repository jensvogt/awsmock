//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_TRANSFER_SERVER_IDENTITY_PROVIDER_DETAILS_H
#define AWSMOCK_DTO_TRANSFER_SERVER_IDENTITY_PROVIDER_DETAILS_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/transfer/model/SftpAuthenticationMethod.h>

namespace AwsMock::Dto::Transfer {

    struct IdentityProviderDetails {

        /**
         * Directory ID
         */
        std::string directoryId;

        /**
         * Lambda function
         */
        std::string function;

        /**
         * IAM Invocation role
         */
        std::string invocationRole;

        /**
         * Identity provider type
         */
        SftpAuthenticationMethod sftpAuthenticationMethod;

        /**
         * Url
         */
        std::string url;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @param jsonString DTO as JSON string
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return JSON object
         */
        void FromDocument(const view_or_value<view, value> &document);

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as JSON string
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
        friend std::ostream &operator<<(std::ostream &os, const IdentityProviderDetails &r);
    };

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_SERVER_IDENTITY_PROVIDER_DETAILS_H
