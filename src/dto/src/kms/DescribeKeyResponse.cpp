//
// Created by vogje01 on 4/25/24.
//

#include <awsmock/dto/kms/DescribeKeyResponse.h>

namespace AwsMock::Dto::KMS {

    std::string DescribeKeyResponse::ToJson() {

        try {

            document document;
            document.append(kvp("KeyMetadata", key.ToDocument()));
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::KMS
