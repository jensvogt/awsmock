//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sqs/intern/ListQueueCountersResponse.h>

namespace AwsMock::Dto::SQS {

    std::string ListQueueCountersResponse::ToJson() const {

        try {

            document document;
            if (!queueCounters.empty()) {
                array queueCounterArrayJson;
                for (const auto &queueCounter: queueCounters) {
                    queueCounterArrayJson.append(queueCounter.ToDocument());
                }
                document.append(kvp("QueueCounters", queueCounterArrayJson));
            }
            Core::Bson::BsonUtils::SetLongValue(document, "Total", total);

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListQueueCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListQueueCountersResponse &r) {
        os << "ListQueueCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
