//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_KMS_DELETE_KEY_REQUEST_H
#define AWSMOCK_DTO_KMS_DELETE_KEY_REQUEST_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::KMS {

    struct DeleteKeyRequest final : Common::BaseCounter<DeleteKeyRequest> {

        /**
         * Key ID
         */
        std::string keyId;

      private:

        friend DeleteKeyRequest tag_invoke(boost::json::value_to_tag<DeleteKeyRequest>, boost::json::value const &v) {
            DeleteKeyRequest r;
            r.keyId = Core::Json::GetStringValue(v, "keyId");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteKeyRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"keyId", obj.keyId},
            };
        }
    };

}// namespace AwsMock::Dto::KMS

#endif// AWSMOCK_CORE_DTO_DELETE_BUCKET_REQUEST_H
