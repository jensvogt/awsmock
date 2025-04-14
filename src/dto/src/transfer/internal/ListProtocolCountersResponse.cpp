//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/transfer/internal/ListProtocolCountersResponse.h>

namespace AwsMock::Dto::Transfer {

    std::string ListProtocolCountersResponse::ToJson() const {

        try {

            document rootDocument;

            if (!protocolCounters.empty()) {
                array protocolArray;
                for (const auto &protocolCounter: protocolCounters) {
                    protocolArray.append(protocolCounter.ToDocument());
                }
                rootDocument.append(kvp("ProtocolCounters", protocolArray));
            }
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "Total", total);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListProtocolCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListProtocolCountersResponse &r) {
        os << "ListProtocolCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
