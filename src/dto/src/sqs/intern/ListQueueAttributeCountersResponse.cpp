//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sqs/intern/ListQueueAttributeCountersResponse.h>

namespace AwsMock::Dto::SQS {

    std::string ListQueueAttributeCountersResponse::ToJson() const {

        try {

            document rootDocument;

            if (!attributeCounters.empty()) {
                array attributeArray;
                for (const auto &attributeCounter: attributeCounters) {
                    attributeArray.append(attributeCounter.ToDocument());
                }
                rootDocument.append(kvp("AttributeCounters", attributeArray));
            }
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "Total", total);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListQueueAttributeCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListQueueAttributeCountersResponse &r) {
        os << "ListQueueAttributeCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
