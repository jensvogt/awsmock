//
// Created by vogje01 on 06/06/2023.
//

#ifndef AWSMOCK_CORE_JWT_UTILS_H
#define AWSMOCK_CORE_JWT_UTILS_H

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#undef BOOST_FILESYSTEM_DYN_LINK
#endif

// C++ standard includes
#include <fcntl.h>
#include <string>

// JWT-cpp includes
#include <jwt-cpp/jwt.h>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>

namespace AwsMock::Core {

    /**
     * @brief JWT token utilities
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class JwtUtils {

      public:

        /**
         * @brief Constructor
         */
        JwtUtils() = default;

        /**
         * @brief Create a new JWT token using Rs256 as signing algorithm
         *
         * @param privateKey RSA private key
         * @param subject subject of the token
         * @param payload payload
         * @return JWT token string
         */
        static std::string CreateTokenRs256(const std::string &privateKey, const std::string &subject, const std::map<std::string, std::string> &payload);

        /**
         * @brief Create a new JWT token using Hs256 as signing algorithm
         *
         * @param privateKey RSA private key
         * @param subject subject of the token
         * @param payload payload
         * @return JWT token string
         */
        static std::string CreateTokenHs256(const std::string &privateKey, const std::string &subject, const std::map<std::string, std::string> &payload);

        /**
         * @brief Verifies a JWT token using Rs256 as signing algorithm
         *
         * @param publicKey RSA public key
         * @param jwt token string
         * @param issuer issuer of the token
         * @return JWT token
         */
        static bool VerifyTokenRs256(const std::string &publicKey, const std::string &jwt, const std::string &issuer);

        /**
         * @brief Verifies a JWT token using Hs256 as signing algorithm
         *
         * @param publicKey RSA public key
         * @param jwt token string
         * @param issuer issuer of the token
         * @return JWT token
         */
        static bool VerifyTokenHs256(const std::string &publicKey, const std::string &jwt, const std::string &issuer);
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_JWT_UTILS_H