//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sns/ListSubscriptionCountersResponse.h>

namespace AwsMock::Dto::SNS {

    std::string ListSubscriptionCountersResponse::ToJson() const {

        try {

            document document;

            if (!subscriptionCounters.empty()) {
                array queueCounterArrayJson;
                for (const auto &subscriptionCounter: subscriptionCounters) {
                    queueCounterArrayJson.append(subscriptionCounter.ToDocument());
                }
                document.append(kvp("SubscriptionCounters", queueCounterArrayJson));
            }
            Core::Bson::BsonUtils::SetLongValue(document, "Total", bsoncxx::types::b_int64(total));
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListSubscriptionCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListSubscriptionCountersResponse &r) {
        os << "ListSubscriptionCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SNS
