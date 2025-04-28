//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/dto/lambda/ListFunctionResponse.h>

namespace AwsMock::Dto::Lambda {

    std::string ListFunctionResponse::ToJson() {

        for (const auto &lambda: lambdaList) {
            Function function = {
                    .codeSha256 = lambda.codeSha256,
                    .codeSize = lambda.memorySize,
                    .functionArn = lambda.arn,
                    .functionName = lambda.function,
                    .handler = lambda.handler,
                    .runtime = lambda.runtime,
                    .lastModified = lambda.modified,
                    .tags = lambda.tags};
            function.environment.variables = lambda.environment.variables;
            functions.push_back(function);
        }

        try {

            document document;

            if (!functions.empty()) {
                array jsonArray;
                for (const auto &function: functions) {
                    jsonArray.append(function.ToDocument());
                }
                document.append(kvp("Functions", jsonArray));
            }

            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::ServiceException(exc.what());
        }
    }

    std::string ListFunctionResponse::ToString() {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, ListFunctionResponse &r) {
        os << "ListFunctionResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::Lambda
