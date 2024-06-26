//
// Created by root on 10/11/23.
//

#ifndef AWSMOCK_DTO_LAMBDA_INVOCATION_NOTIFICATION_H
#define AWSMOCK_DTO_LAMBDA_INVOCATION_NOTIFICATION_H

// C++ includes
#include <sstream>
#include <string>

// Poco includes
#include <Poco/Notification.h>

namespace AwsMock::Dto::Lambda {

    struct [[maybe_unused]] InvocationNotification : public Poco::Notification {

        /**
         * Function name
         */
        std::string functionName;

        /**
         * Payload
         */
        std::string payload;

        /**
         * AWS region
         */
        std::string region;

        /**
         * AWS user
         */
        std::string user;

        /**
         * Host name
         */
        std::string hostName;

        /**
         * Host name
         */
        int port;

        /**
         * Constructor
         *
         * @param functionName name of the lambda function
         * @param payload event payload
         * @param region AWS region
         * @param user AWS user
         * @param hostName hostname of the lambda function
         * @param port lambda function port
         */
        InvocationNotification(const std::string &functionName, const std::string &payload, const std::string &region, const std::string &user, const std::string &hostName, int port);

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
        friend std::ostream &operator<<(std::ostream &os, const InvocationNotification &i);
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_INVOCATION_NOTIFICATION_H
