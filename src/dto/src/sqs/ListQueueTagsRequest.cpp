//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sqs/ListQueueTagsRequest.h>

namespace AwsMock::Dto::SQS {

    void ListQueueTagsRequest::FromJson(const std::string &jsonString) {

        try {
            const value document = bsoncxx::from_json(jsonString);

            region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
            queueUrl = Core::Bson::BsonUtils::GetStringValue(document, "QueueUrl");
            pageSize = Core::Bson::BsonUtils::GetIntValue(document, "PageSize");
            pageIndex = Core::Bson::BsonUtils::GetIntValue(document, "PageIndex");

            // Sort columns
            if (document.find("SortColumns") != document.end()) {

                for (const bsoncxx::array::view arrayView{document["SortColumns"].get_array().value}; const bsoncxx::array::element &element: arrayView) {
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

    std::string ListQueueTagsRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "Region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "QueueUrl", queueUrl);
            Core::Bson::BsonUtils::SetIntValue(document, "PageSize", pageSize);
            Core::Bson::BsonUtils::SetIntValue(document, "PageIndex", pageIndex);

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

    std::string ListQueueTagsRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListQueueTagsRequest &r) {
        os << "ListQueueTagsRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
