//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/lambda/intern/ListLambdaEnvironmentCountersResponse.h>

namespace AwsMock::Dto::Lambda {

    std::string ListLambdaEnvironmentCountersResponse::ToJson() const {

        try {

            document rootDocument;

            if (!environmentCounters.empty()) {
                array tagsArray;
                for (const auto &[fst, snd]: environmentCounters) {
                    document tagsDocument;
                    tagsDocument.append(kvp("key", fst));
                    tagsDocument.append(kvp("value", snd));
                    tagsArray.append(tagsDocument);
                }
                rootDocument.append(kvp("EnvironmentCounters", tagsArray));
            }
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "Total", total);

            std::string tmp = Core::Bson::BsonUtils::ToJsonString(rootDocument);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListLambdaEnvironmentCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListLambdaEnvironmentCountersResponse &r) {
        os << "ListLambdaEnvironmentCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
