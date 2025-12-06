//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/transfer/ListUserCountersResponse.h>

namespace AwsMock::Dto::Transfer {

    std::string ListUserCountersResponse::ToJson() const {

        try {

            document rootDocument;

            if (!userCounters.empty()) {
                array attributeArray;
                for (const auto &userCounter: userCounters) {
                    attributeArray.append(userCounter.ToDocument());
                }
                rootDocument.append(kvp("UserCounters", attributeArray));
            }
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "Total", total);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListUserCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListUserCountersResponse &r) {
        os << "ListUserCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Transfer
