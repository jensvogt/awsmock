
#include <awsmock/dto/transfer/internal/ListServerCountersResponse.h>

namespace AwsMock::Dto::Transfer {

    std::string ListServerCountersResponse::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "Total", total);

            // Contents
            if (!transferServers.empty()) {

                array jsonBucketArray;
                for (const auto &transferServer: transferServers) {
                    jsonBucketArray.append(transferServer.ToDocument());
                }

                rootDocument.append(kvp("TransferServerCounters", jsonBucketArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    [[nodiscard]] std::string ListServerCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListServerCountersResponse &r) {
        os << "ListServerCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer