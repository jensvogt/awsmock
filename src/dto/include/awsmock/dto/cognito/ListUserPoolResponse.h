//
// Created by vogje01 on 11/25/23.
//

#ifndef AWSMOCK_DTO_COGNITO_LIST_USERPOOL_RESPONSE_H
#define AWSMOCK_DTO_COGNITO_LIST_USERPOOL_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/dto/cognito/model/UserPool.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::Cognito {

    /**
     * @brief List user pool response
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ListUserPoolResponse : public Dto::Common::BaseRequest {

        /**
         * User pool entities
         */
        std::vector<UserPool> userPools;

        /**
         * Total number of user pools
         */
        long total;

        /**
         * Convert to a JSON string.
         *
         * @return user pools json string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ListUserPoolResponse &i);
    };

}// namespace AwsMock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_LIST_USERPOOL_RESPONSE_H
