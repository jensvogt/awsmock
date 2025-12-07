//
// Created by vogje01 on 03/09/2023.
//

#ifndef AWSMOCK_DTO_LAMBDA_EPHEMERAL_STORAGE_H
#define AWSMOCK_DTO_LAMBDA_EPHEMERAL_STORAGE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/logging/LogStream.h>

// TODO: Split into header code
namespace AwsMock::Dto::Lambda {

    struct EphemeralStorage final : Common::BaseCounter<EphemeralStorage> {

        /**
         * Temporary disk space in MB. Default: 512 MB, Range: 512 - 10240 MB
         */
        long size = 512;

        /**
         * @brief Convert to a JSON object
         *
         * @return JSON object
         */
        view_or_value<view, value> ToDocument() const {

            try {
                document document;
                Core::Bson::BsonUtils::SetLongValue(document, "Size", size);
                return view_or_value<view, value>(document);

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Convert to a JSON string
         *
         * @param document JSON string
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {
                size = Core::Bson::BsonUtils::GetLongValue(document, "Size");
            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend EphemeralStorage tag_invoke(boost::json::value_to_tag<EphemeralStorage>, boost::json::value const &v) {
            EphemeralStorage r;
            r.size = Core::Json::GetLongValue(v, "Size");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, EphemeralStorage const &obj) {
            jv = {
                    {"Size", obj.size},
            };
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_EPHEMERAL_STORAGE_H
