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

    struct EphemeralStorage {

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

        /**
         * @brief Converts the DTO to a string representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] std::string ToString() const {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        /**
         * @brief Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const EphemeralStorage &r) {
            os << "EphemeralStorage=" << to_json(r.ToDocument());
            return os;
        }
    };

}// namespace AwsMock::Dto::Lambda

#endif// AWSMOCK_DTO_LAMBDA_EPHEMERAL_STORAGE_H
