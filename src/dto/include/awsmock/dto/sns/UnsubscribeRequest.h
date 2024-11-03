//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SNS_UNSUBSCRIBE_REQUEST_H
#define AWSMOCK_DTO_SNS_UNSUBSCRIBE_REQUEST_H

// C++ standard includes
#include <sstream>
#include <string>

namespace AwsMock::Dto::SNS {

    /**
     * @brief Unsubscribe a method from the topic
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct UnsubscribeRequest {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Subscription ARN
         */
        std::string subscriptionArn;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const UnsubscribeRequest &r);
    };

}// namespace AwsMock::Dto::SNS

#endif// AWSMOCK_DTO_SNS_UNSUBSCRIBE_REQUEST_H
