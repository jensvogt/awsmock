//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/lambda/intern/ListLambdaTagCountersResponse.h>

namespace AwsMock::Dto::Lambda {

    std::string ListLambdaTagCountersResponse::ToJson() const {

        try {

            document rootDocument;

            if (!tagCounters.empty()) {
                array tagsArray;
                for (const auto &[fst, snd]: tagCounters) {
                    document tagsDocument;
                    tagsDocument.append(kvp("key", fst));
                    tagsDocument.append(kvp("value", snd));
                    tagsArray.append(tagsDocument);
                }
                rootDocument.append(kvp("TagCounters", tagsArray));
            }
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "Total", total);

            std::string tmp = Core::Bson::BsonUtils::ToJsonString(rootDocument);
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListLambdaTagCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListLambdaTagCountersResponse &r) {
        os << "ListLambdaTagCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
