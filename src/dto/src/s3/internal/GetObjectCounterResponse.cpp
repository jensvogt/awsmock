
#include <awsmock/dto/s3/internal/GetObjectCounterResponse.h>

namespace AwsMock::Dto::S3 {

    std::string GetObjectCounterResponse::ToJson() const {

        try {

            document rootDocument;
            rootDocument.append(kvp("objectCounter", objectCounter.ToDocument()));
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    [[nodiscard]] std::string GetObjectCounterResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetObjectCounterResponse &r) {
        os << "GetObjectCounterResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::S3