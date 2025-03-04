
#include <awsmock/dto/s3/internal/ListBucketCounterResponse.h>

namespace AwsMock::Dto::S3 {

    std::string ListBucketCounterResponse::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "total", total);

            // Contents
            if (!bucketCounters.empty()) {

                array jsonBucketArray;
                for (const auto &bucketCounter: bucketCounters) {
                    jsonBucketArray.append(bucketCounter.ToDocument());
                }

                rootDocument.append(kvp("bucketCounters", jsonBucketArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    [[nodiscard]] std::string ListBucketCounterResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListBucketCounterResponse &r) {
        os << "ListBucketCounterResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3