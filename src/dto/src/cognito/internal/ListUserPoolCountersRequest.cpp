//
// Created by vogje01 on 11/25/23.
//

#include <awsmock/dto/cognito/internal/ListUserPoolCountersRequest.h>

namespace AwsMock::Dto::Cognito {

    void ListUserPoolCountersRequest::FromJson(const std::string &payload) {

        try {

            const value documentValue = bsoncxx::from_json(payload);
            region = Core::Bson::BsonUtils::GetStringValue(documentValue, "region");
            prefix = Core::Bson::BsonUtils::GetStringValue(documentValue, "prefix");
            pageSize = Core::Bson::BsonUtils::GetIntValue(documentValue, "pageSize");
            pageIndex = Core::Bson::BsonUtils::GetIntValue(documentValue, "pageIndex");

            if (documentValue.find("sortColumns") != documentValue.end()) {

                for (const bsoncxx::array::view arrayView{documentValue["sortColumns"].get_array().value}; const bsoncxx::array::element &element: arrayView) {
                    Database::SortColumn sortColumn;
                    sortColumn.FromDocument(element.get_document());
                    sortColumns.emplace_back(sortColumn);
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListUserPoolCountersRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "prefix", prefix);
            Core::Bson::BsonUtils::SetIntValue(document, "pageSize", pageSize);
            Core::Bson::BsonUtils::SetIntValue(document, "pageIndex", pageIndex);

            if (!sortColumns.empty()) {
                array jsonArray;
                for (const auto &sortColumn: sortColumns) {
                    jsonArray.append(sortColumn.ToDocument());
                }
                document.append(kvp("sortColumns", jsonArray));
            }

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

}// namespace AwsMock::Dto::Cognito