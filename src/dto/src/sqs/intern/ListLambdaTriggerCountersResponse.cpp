//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sqs/internal/ListLambdaTriggerCountersResponse.h>

namespace AwsMock::Dto::SQS {

    std::string ListLambdaTriggerCountersResponse::ToJson() const {

        try {

            document rootDocument;

            if (!lambdaTriggerCounters.empty()) {
                array attributeArray;
                for (const auto &lambdaTrigger: lambdaTriggerCounters) {
                    attributeArray.append(lambdaTrigger.ToDocument());
                }
                rootDocument.append(kvp("LambdaTriggerCounters", attributeArray));
            }
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "Total", bsoncxx::types::b_int64(total));
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListLambdaTriggerCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListLambdaTriggerCountersResponse &r) {
        os << "ListLambdaTriggerCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
