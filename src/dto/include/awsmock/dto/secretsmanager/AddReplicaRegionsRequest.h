//
// Created by vogje01 on 4/8/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_ADD_REPLICA_REGIONS_REQUESTH
#define AWSMOCK_DTO_SECRETSMANAGER_ADD_REPLICA_REGIONS_REQUESTH

// C++ standard includes
#include <string>

// AwsMoc includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Secrets manager replication status.
     *
     * Example:
     * @code{.json}
     * "AddReplicaRegions": [
     *   {
     *     "KmsKeyId": "string",
     *     "Region": "string"
     *   }
     * ],
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct AddReplicaRegionsRequest final : Common::BaseCounter<AddReplicaRegionsRequest> {

        /**
         * KMS key ID
         */
        std::string kmsKeyId;

        /**
         * @brief Converts the DTO to a JSON object.
         *
         * @return DTO as JSON object
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
                Core::Bson::BsonUtils::SetStringValue(document, "KmsKeyId", kmsKeyId);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend AddReplicaRegionsRequest tag_invoke(boost::json::value_to_tag<AddReplicaRegionsRequest>, boost::json::value const &v) {
            AddReplicaRegionsRequest r;
            r.kmsKeyId = Core::Json::GetStringValue(v, "KmsKeyId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, AddReplicaRegionsRequest const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"KmsKeyId", obj.kmsKeyId},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif// AWSMOCK_DTO_SECRETSMANAGER_ADD_REPLICA_REGIONS_REQUESTH
