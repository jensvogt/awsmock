//
// Created by vogje01 on 31/05/2023.
//

#include <awsmock/dto/s3/internal/ListBucketCounterRequest.h>

namespace AwsMock::Dto::S3 {

    ListBucketCounterRequest ListBucketCounterRequest::FromJson(const std::string &body) {

        try {
            ListBucketCounterRequest request;

            const value document = bsoncxx::from_json(body);
            request.region = Core::Bson::BsonUtils::GetStringValue(document, "region");
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

    std::string ListBucketCounterRequest::ToJson() const {

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

    std::string ListBucketCounterRequest::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListBucketCounterRequest &r) {
        os << "ListBucketCounterRequest=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3
