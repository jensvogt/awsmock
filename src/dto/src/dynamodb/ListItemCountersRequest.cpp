//
// Created by vogje01 on 20/12/2023.
//

#include <awsmock/dto/dynamodb/ListItemCountersRequest.h>

namespace AwsMock::Dto::DynamoDb {

    std::string ListItemCountersRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "tableName", tableName);
            Core::Bson::BsonUtils::SetStringValue(document, "prefix", prefix);
            Core::Bson::BsonUtils::SetIntValue(document, "pageSize", pageSize);
            Core::Bson::BsonUtils::SetIntValue(document, "pageIndex", pageIndex);

            // Sort columns
            if (!sortColumns.empty()) {
                array jsonArray;
                for (const auto &sortColumn: sortColumns) {
                    jsonArray.append(sortColumn.ToDocument());
                }
                document.append(kvp("sortColumns", jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void ListItemCountersRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "region");
            tableName = Core::Bson::BsonUtils::GetStringValue(document, "tableName");
            prefix = Core::Bson::BsonUtils::GetStringValue(document, "prefix");
            pageSize = Core::Bson::BsonUtils::GetIntValue(document, "pageSize");
            pageIndex = Core::Bson::BsonUtils::GetIntValue(document, "pageIndex");

            // Sort columns
            if (document.find("sortColumns") != document.end()) {

                for (const bsoncxx::array::view arrayView{document["sortColumns"].get_array().value}; const bsoncxx::array::element &element: arrayView) {
                    Core::SortColumn sortColumn;
                    sortColumn.FromDocument(element.get_document());
                    sortColumns.emplace_back(sortColumn);
                }
            }

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListItemCountersRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListItemCountersRequest &r) {
        os << "ListItemCountersRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::DynamoDb
