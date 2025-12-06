//
// Created by vogje01 on 5/1/25.
//

#ifndef AWSMOCK_DB_SQS_UTILS_H
#define AWSMOCK_DB_SQS_UTILS_H

// C++ includes
#include <map>
#include <string>

// AwsMock includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/entity/sqs/MessageAttribute.h>

namespace AwsMock::Database {

    class SqsUtils {

      public:

        /**
         * @brief Calculates the MD5 of the message body
         *
         * @param messageBody body of the message
         * @return MD5 sum of message body
         */
        static std::string CreateMd5OfMessageBody(const std::string &messageBody);

        /**
         * @brief Calculates the MD5 of the message attributes
         *
         * @param messageAttributes list of message attributes
         * @return MD5 sum of message attributes
         */
        static std::string CreateMd5OfMessageAttributes(const std::map<std::string, Entity::SQS::MessageAttribute> &messageAttributes);

      private:

        /**
         * @brief Converts an integer to a byte array
         *
         * @param n size of the byte array
         * @param bytes output bytes
         */
        static void GetIntAsByteArray(size_t n, unsigned char *bytes);

        /**
         * @brief Updates the crypto string with value and length
         *
         * @param context SSL context
         * @param str input string
         */
        static void UpdateLengthAndBytes(EVP_MD_CTX *context, const std::string &str);
    };

}// namespace AwsMock::Database

#endif//AWSMOCK_DB_SQS_UTILS_H
