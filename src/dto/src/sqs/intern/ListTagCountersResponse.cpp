//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sqs/intern/ListTagCountersResponse.h>

namespace AwsMock::Dto::SQS {

    std::string ListTagCountersResponse::ToJson() const {

        try {

            document rootDocument;

            if (!tagCounters.empty()) {
                array tagArray;
                for (const auto &tagCounter: tagCounters) {
                    tagArray.append(tagCounter.ToDocument());
                }
                rootDocument.append(kvp("TagCounters", tagArray));
            }
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "Total", total);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListTagCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListTagCountersResponse &r) {
        os << "ListTagCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
