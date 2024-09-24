//
// Created by vogje01 on 18/05/2023.
//

#include <awsmock/core/JwtUtils.h>

namespace AwsMock::Core {

    std::string JwtUtils::CreateTokenRs256(const std::string &privateKey, const std::string &issuer, const std::map<std::string, std::string> &payload) {

        auto token = jwt::create()
                             .set_issuer(issuer)
                             .set_type("JWT")
                             .set_id("rsa-create-example")
                             .set_issued_now()
                             .set_expires_in(std::chrono::seconds{36000});
        for (const auto &p: payload) {
            token.set_payload_claim(p.first, jwt::claim(p.second));
        }
        return token.sign(jwt::algorithm::rs256("", privateKey, "", ""));
    }

    std::string JwtUtils::CreateTokenHs256(const std::string &secret, const std::string &issuer, const std::map<std::string, std::string> &payload) {

        auto token = jwt::create()
                             .set_issuer(issuer)
                             .set_type("JWT")
                             .set_id("rsa-create-example")
                             .set_issued_now()
                             .set_expires_in(std::chrono::seconds{36000});
        for (const auto &p: payload) {
            token.set_payload_claim(p.first, jwt::claim(p.second));
        }
        return token.sign(jwt::algorithm::rs256(secret));
    }

    bool JwtUtils::VerifyTokenRs256(const std::string &publicKey, const std::string &jwt, const std::string &issuer) {
        try {
            auto verify = jwt::verify()
                                  // We only need an RSA public key to verify tokens
                                  .allow_algorithm(jwt::algorithm::rs256(publicKey, "", "", ""))
                                  // We expect token to come from a known authorization server
                                  .with_issuer(issuer);
        } catch (jwt::error::token_verification_error &ex) {
            log_error << "Invalid JWT token, code: " << make_error_code(ex);
            return false;
        }
        return true;
    }

    bool JwtUtils::VerifyTokenHs256(const std::string &secret, const std::string &jwt, const std::string &issuer) {
        try {
            auto verify = jwt::verify()
                                  // We only need an RSA public key to verify tokens
                                  .allow_algorithm(jwt::algorithm::hs256(secret))
                                  // We expect token to come from a known authorization server
                                  .with_issuer(issuer);
        } catch (jwt::error::token_verification_error &ex) {
            log_error << "Invalid JWT token, code: " << make_error_code(ex);
            return false;
        }
        return true;
    }

}// namespace AwsMock::Core
