//
// Created by vogje01 on 31/05/2023.
//

#include <awsmock/dto/s3/internal/ListObjectCounterRequest.h>

namespace AwsMock::Dto::S3 {

    ListObjectCounterRequest ListObjectCounterRequest::FromJson(const std::string &body) {

        try {
            ListObjectCounterRequest request;

            const value document = bsoncxx::from_json(body);
            request.region = Core::Bson::BsonUtils::GetStringValue(document, "region");
            request.bucket = Core::Bson::BsonUtils::GetStringValue(document, "bucket");
            request.prefix = Core::Bson::BsonUtils::GetStringValue(document, "prefix");
            request.pageSize = Core::Bson::BsonUtils::GetIntValue(document, "pageSize");
            request.pageIndex = Core::Bson::BsonUtils::GetIntValue(document, "pageIndex");

            if (document.find("sortColumns") != document.end()) {

                for (const bsoncxx::array::view arrayView{document["sortColumns"].get_array().value}; const bsoncxx::array::element &element: arrayView) {
                    Core::SortColumn sortColumn;
                    sortColumn.FromDocument(element.get_document());
                    request.sortColumns.emplace_back(sortColumn);
                }
            }
            return request;

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListObjectCounterRequest::ToJson() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "region", region);
            Core::Bson::BsonUtils::SetStringValue(document, "prefix", prefix);
            Core::Bson::BsonUtils::SetStringValue(document, "bucket", bucket);
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

    std::string ListObjectCounterRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListObjectCounterRequest &r) {
        os << "ListObjectCounterRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3
