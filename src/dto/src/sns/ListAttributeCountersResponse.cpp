//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sns/ListAttributeCountersResponse.h>

namespace AwsMock::Dto::SNS {

    std::string ListAttributeCountersResponse::ToJson() const {

        try {

            document rootDocument;

            if (!attributeCounters.empty()) {
                array attributeArray;
                for (const auto &attributeCounter: attributeCounters) {
                    attributeArray.append(attributeCounter.ToDocument());
                }
                rootDocument.append(kvp("AttributeCounters", attributeArray));
            }
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "Total", bsoncxx::types::b_int64(total));
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListAttributeCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListAttributeCountersResponse &r) {
        os << "ListAttributeCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SNS
