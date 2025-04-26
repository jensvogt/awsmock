//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_GET_ITEM_KEY_H
#define AWSMOCK_DTO_DYNAMODB_GET_ITEM_KEY_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseDto.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB get item key
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetItemKey final : Common::BaseDto<GetItemKey> {

        /**
         * Key type
         */
        std::string keyType;

        /**
         * Key value
         */
        std::string keyValue;

        /**
         * @brief Parse a JSON stream
         *
         * @param jsonBody JSON string
         */
        void FromJson(const std::string &jsonBody);

        /**
         * @brief Creates a JSON string from the object.
         *
         * @return JSON string
         */
        std::string ToJson() const override;
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_GET_ITEM_KEY_H
