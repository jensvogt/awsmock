//
// Created by vogje01 on 30/05/2023.
//

#include <awsmock/dto/monitoring/GetMultiCountersResponse.h>

namespace AwsMock::Dto::Monitoring {

    std::string GetMultiCountersResponse::ToJson() const {

        try {

            document rootDocument;

            for (const auto &[fst, snd]: counters) {
                array jsonArray;
                for (const auto &value: snd) {
                    array jsonElement;
                    jsonElement.append(Core::DateTimeUtils::ToISO8601(value.timestamp));
                    jsonElement.append(value.performanceValue);
                    jsonArray.append(jsonElement);
                }
                rootDocument.append(kvp(fst, jsonArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (Core::JsonException &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string GetMultiCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const GetMultiCountersResponse &r) {
        os << "GetMultiCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Monitoring
