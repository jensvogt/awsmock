//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_TRANSFER_CREATE_TRANSFER_REQUEST_H
#define AWSMOCK_DTO_TRANSFER_CREATE_TRANSFER_REQUEST_H

// C++ standard includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/transfer/model/IdentityProviderDetails.h>
#include <awsmock/dto/transfer/model/Protocol.h>


namespace AwsMock::Dto::Transfer {

    struct CreateServerRequest {

        /**
         * Region
         */
        std::string region;

        /**
         * Domain
         */
        std::string domain;

        /**
         * Protocols
         */
        std::vector<ProtocolType> protocols;

        /**
         * Tags
         */
        std::map<std::string, std::string> tags;

        /**
         * Identity provider details
         */
        IdentityProviderDetails identityProviderDetails;

        /**
         * @brief Parse a JSON stream
         *
         * @param jsonString json input stream
         */
        void FromJson(const std::string &jsonString);

        /**
         * @brief Creates a JSON string from the object.
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
        friend std::ostream &operator<<(std::ostream &os, const CreateServerRequest &r);
    };

}// namespace AwsMock::Dto::Transfer

#endif