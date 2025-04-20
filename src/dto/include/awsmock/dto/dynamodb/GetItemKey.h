//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_GET_ITEM_KEY_H
#define AWSMOCK_DTO_DYNAMODB_GET_ITEM_KEY_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseRequest.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB get item key
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct GetItemKey : Common::BaseRequest<GetItemKey> {

        /**
         * Region
         */
        std::string type;

        /**
         * Table name
         */
        std::string value;

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
        std::string ToJson() override;
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_GET_ITEM_KEY_H
