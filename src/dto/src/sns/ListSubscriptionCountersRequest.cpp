//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sns/ListSubscriptionCountersRequest.h>

namespace AwsMock::Dto::SNS {

    void ListSubscriptionCountersRequest::FromJson(const std::string &jsonString) {


        try {
            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "region");
            topicArn = Core::Bson::BsonUtils::GetStringValue(document, "topicArn");
            prefix = Core::Bson::BsonUtils::GetStringValue(document, "prefix");
            pageSize = Core::Bson::BsonUtils::GetIntValue(document, "pageSize");
            pageIndex = Core::Bson::BsonUtils::GetIntValue(document, "pageIndex");

            if (document.find("sortColumns") != document.end()) {

                for (const bsoncxx::array::view arrayView{document["sortColumns"].get_array().value}; const bsoncxx::array::element &element: arrayView) {
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

    std::string ListSubscriptionCountersRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "topicArn", topicArn);
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

    std::string ListSubscriptionCountersRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListSubscriptionCountersRequest &r) {
        os << "ListSubscriptionCountersRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SNS
