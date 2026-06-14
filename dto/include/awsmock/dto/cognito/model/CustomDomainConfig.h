//
// Created by vogje01 on 5/27/24.
//

#ifndef AWSMOCK_DTO_COGNITO_CUSTOM_DOMAIN_CONFIG_H
#define AWSMOCK_DTO_COGNITO_CUSTOM_DOMAIN_CONFIG_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseObject.h>

namespace Awsmock::Dto::Cognito {

    /**
     * @brief Custom domain config
     *
     * @code{.json}
     * "CustomDomainConfig": {
     *     "CertificateArn": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct CustomDomainConfig final : Common::BaseObject<CustomDomainConfig> {

        /**
         * CertificateArn
         */
        std::string certificateArn;

        /**
         * @brief Convert from a JSON object.
         *
         * @param document json object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                certificateArn = Core::Bson::BsonUtils::GetStringValue(document, "CertificateArn");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {
            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "CertificateArn", certificateArn);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        mutable logger_t _logger{boost::log::keywords::channel = "Cognito"};

        friend CustomDomainConfig tag_invoke(boost::json::value_to_tag<CustomDomainConfig>, boost::json::value const &v) {
            CustomDomainConfig r;
            r.certificateArn = Core::Json::GetStringValue(v, "CertificateArn");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, CustomDomainConfig const &obj) {
            jv = {
                    {"CertificateArn", obj.certificateArn},
            };
        }
    };

}// namespace Awsmock::Dto::Cognito

#endif// AWSMOCK_DTO_COGNITO_CUSTOM_DOMAIN_CONFIG_H
