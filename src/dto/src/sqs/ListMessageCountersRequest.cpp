//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sqs/ListMessageCountersRequest.h>

namespace AwsMock::Dto::SQS {

    void ListMessageCountersRequest::FromJson(const std::string &jsonString) {

        try {

            const value document = bsoncxx::from_json(jsonString);

            queueArn = Core::Bson::BsonUtils::GetStringValue(document, "queueArn");
            prefix = Core::Bson::BsonUtils::GetStringValue(document, "prefix");
            pageSize = Core::Bson::BsonUtils::GetIntValue(document, "pageSize");
            pageIndex = Core::Bson::BsonUtils::GetIntValue(document, "pageIndex");

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

    std::string ListMessageCountersRequest::ToJson() const {

        try {

            document document;
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

    std::string ListMessageCountersRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListMessageCountersRequest &r) {
        os << "ListMessageCountersRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS