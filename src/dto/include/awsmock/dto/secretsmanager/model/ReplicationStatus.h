//
// Created by vogje01 on 4/8/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_REPLICATION_STATUS_H
#define AWSMOCK_DTO_SECRETSMANAGER_REPLICATION_STATUS_H

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Secrets manager replication status.
     *
     * Example:
     * @code{.json}
     * {
     *   "KmsKeyId": "string",
     *   "LastAccessedDate": number,
     *   "Region": "string",
     *   "Status": "string",
     *   "StatusMessage": "string"
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ReplicationStatus final : Common::BaseCounter<ReplicationStatus> {

        /**
         * ARN
         */
        std::string arn;

        /**
         * KMS key ID
         */
        std::string kmsKeyId;

        /**
         * Last access date
         */
        long lastAccessedDate{};

        /**
         * Status
         */
        std::string status;

        /**
         * Status message
         */
        std::string statusMessage;

        /**
         * @brief Converts the DTO to a JSON object.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
                Core::Bson::BsonUtils::SetStringValue(document, "ARN", arn);
                Core::Bson::BsonUtils::SetStringValue(document, "KmsKeyId", kmsKeyId);
                Core::Bson::BsonUtils::SetLongValue(document, "LastAccessedDate", lastAccessedDate);
                Core::Bson::BsonUtils::SetStringValue(document, "Status", status);
                Core::Bson::BsonUtils::SetStringValue(document, "StatusMessage", statusMessage);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON representation to s DTO.
         *
         * @param document JSON object.
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
                arn = Core::Bson::BsonUtils::GetStringValue(document, "ARN");
                kmsKeyId = Core::Bson::BsonUtils::GetStringValue(document, "KmsKeyId");
                lastAccessedDate = Core::Bson::BsonUtils::GetLongValue(document, "LastAccessedDate");
                status = Core::Bson::BsonUtils::GetStringValue(document, "Status");
                statusMessage = Core::Bson::BsonUtils::GetStringValue(document, "StatusMessage");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend ReplicationStatus tag_invoke(boost::json::value_to_tag<ReplicationStatus>, boost::json::value const &v) {
            ReplicationStatus r;
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.kmsKeyId = Core::Json::GetStringValue(v, "KmsKeyId");
            r.lastAccessedDate = Core::Json::GetLongValue(v, "LastAccessedDate");
            r.status = Core::Json::GetStringValue(v, "Status");
            r.statusMessage = Core::Json::GetStringValue(v, "StatusMessage");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, ReplicationStatus const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"KmsKeyId", obj.kmsKeyId},
                    {"LastAccessedDate", obj.lastAccessedDate},
                    {"Status", obj.status},
                    {"StatusMessage", obj.statusMessage},
            };
        }
    };

}//namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_REPLICATION_STATUS_H
