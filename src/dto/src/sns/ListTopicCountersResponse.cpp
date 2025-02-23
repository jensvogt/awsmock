//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sns/ListTopicCountersResponse.h>

namespace AwsMock::Dto::SNS {

    std::string ListTopicCountersResponse::ToJson() const {

        try {

            document document;
            if (!topicCounters.empty()) {
                array topicCounterArrayJson;
                for (const auto &topicCounter: topicCounters) {
                    topicCounterArrayJson.append(topicCounter.ToDocument());
                }
                document.append(kvp("TopicCounters", topicCounterArrayJson));
            }
            document.append(kvp("Total", bsoncxx::types::b_int64(total)));

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListTopicCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListTopicCountersResponse &r) {
        os << "ListTopicCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SNS
