
#include <awsmock/dto/lambda/intern/ListFunctionCountersResponse.h>

namespace AwsMock::Dto::Lambda {

    std::string ListFunctionCountersResponse::ToJson() const {

        try {

            document rootDocument;
            Core::Bson::BsonUtils::SetLongValue(rootDocument, "total", total);

            // Contents
            if (!functionCounters.empty()) {

                array jsonBucketArray;
                for (auto &functionCounter: functionCounters) {
                    document jsonObject;
                    jsonObject.append(kvp("functionName", functionCounter.functionName));
                    jsonObject.append(kvp("functionArn", functionCounter.functionArn));
                    jsonObject.append(kvp("runtime", functionCounter.runtime));
                    jsonObject.append(kvp("handler", functionCounter.handler));
                    jsonObject.append(kvp("version", functionCounter.version));
                    jsonObject.append(kvp("state", functionCounter.state));
                    jsonObject.append(kvp("averageRuntime", bsoncxx::types::b_int64(functionCounter.averageRuntime)));
                    jsonObject.append(kvp("invocations", bsoncxx::types::b_int64(functionCounter.invocations)));
                    jsonBucketArray.append(jsonObject);
                }
                rootDocument.append(kvp("functionCounters", jsonBucketArray));
            }
            return Core::Bson::BsonUtils::ToJsonString(rootDocument);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    [[nodiscard]] std::string ListFunctionCountersResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListFunctionCountersResponse &r) {
        os << "ListFunctionCountersResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda