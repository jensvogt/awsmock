//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sqs/internal/ListLambdaTriggerCountersRequest.h>

namespace AwsMock::Dto::SQS {

    void ListLambdaTriggerCountersRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);
            region = Core::Bson::BsonUtils::GetStringValue(document, "region");
            queueArn = Core::Bson::BsonUtils::GetStringValue(document, "queueArn");
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

    std::string ListLambdaTriggerCountersRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "queueArn", queueArn);
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

    std::string ListLambdaTriggerCountersRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListLambdaTriggerCountersRequest &r) {
        os << "ListLambdaTriggerCountersRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
