//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/transfer/internal/ListServerCountersRequest.h>

namespace AwsMock::Dto::Transfer {

    std::string ListServerCountersRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "Prefix", prefix);
            Core::Bson::BsonUtils::SetIntValue(document, "PageSize", pageSize);
            Core::Bson::BsonUtils::SetIntValue(document, "PageIndex", pageIndex);

            if (!sortColumns.empty()) {
                array jsonArray;
                for (const auto &sortColumn: sortColumns) {
                    jsonArray.append(sortColumn.ToDocument());
                }
                document.append(kvp("SortColumns", jsonArray));
            }

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void ListServerCountersRequest::FromJson(const std::string &jsonString) {

        try {
            const value rootDocument = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Region");
            prefix = Core::Bson::BsonUtils::GetStringValue(rootDocument, "Prefix");
            pageSize = Core::Bson::BsonUtils::GetIntValue(rootDocument, "PageSize");
            pageIndex = Core::Bson::BsonUtils::GetIntValue(rootDocument, "PageIndex");

            if (rootDocument.find("SortColumns") != rootDocument.end()) {

                for (const bsoncxx::array::view arrayView{rootDocument["sortColumns"].get_array().value}; const bsoncxx::array::element &element: arrayView) {
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

    std::string ListServerCountersRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListServerCountersRequest &r) {
        os << "ListServerCountersRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
