//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_DYNAMODB_EXPORT_ITEMS_RESPONSE_H
#define AWSMOCK_DTO_DYNAMODB_EXPORT_ITEMS_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include "awsmock/dto/module/model/ExportType.h"
#include "awsmock/dto/module/model/Infrastructure.h"


#include <awsmock/core/JsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/entity/dynamodb/Item.h>
#include <awsmock/entity/dynamodb/Table.h>

namespace AwsMock::Dto::DynamoDb {

    /**
     * @brief DynamoDB list table counters request
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct ExportItemsResponse final : Common::BaseCounter<ExportItemsResponse> {

        /**
         * AWS region
         */
        std::string region;

        /**
         * Table name
         */
        std::string tableName;

        /**
         * ExportType
         */
        Module::ExportType exportType = Module::OBJECTS;

        /**
         * ExportType
         */
        bool prettyPrint = true;

        /**
         * Infrastructure
         */
        Module::Infrastructure infrastructure;

        /**
         * @brief Convert from a JSON object.
         *
         * @return payload json string
         */
        [[nodiscard]] std::string ToJson() const override {
            try {

                document document;
                document.append(kvp("exportType", Module::ExportTypeToString(exportType)));
                document.append(kvp("prettyPrint", prettyPrint));
                document.append(kvp("infrastructure", infrastructure.ToDocument()));
                return bsoncxx::to_json(document);

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }
    };

}// namespace AwsMock::Dto::DynamoDb

#endif// AWSMOCK_DTO_DYNAMODB_EXPORT_ITEMS_RESPONSE_H
