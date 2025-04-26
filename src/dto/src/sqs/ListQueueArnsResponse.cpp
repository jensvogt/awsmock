//
// Created by vogje01 on 23/09/2023.
//

#include <awsmock/dto/sqs/ListQueueArnsResponse.h>

namespace AwsMock::Dto::SQS {

    std::string ListQueueArnsResponse::ToXml() const {

        try {
            boost::property_tree::ptree root;
            if (!queueArns.empty()) {
                for (const auto &queueArn: queueArns) {
                    root.add("GetQueueUrlResponse.GetQueueUrlResult.QueueArn", queueArn);
                }
            }
            root.add("GetQueueUrlResponse.ResponseMetadata.RequestId", Core::StringUtils::CreateRandomUuid());
            return Core::XmlUtils::ToXmlString(root);

        } catch (std::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListQueueArnsResponse::ToJson() const {

        try {

            document document;

            if (!queueArns.empty()) {
                array arnsArrayJson;
                for (const auto &queueArn: queueArns) {
                    arnsArrayJson.append(queueArn);
                }
                document.append(kvp("QueueArns", arnsArrayJson));
            }
            return Core::Bson::BsonUtils::ToJsonString(document);

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    std::string ListQueueArnsResponse::ToString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const ListQueueArnsResponse &r) {
        os << "ListQueueArnsResponse=" << r.ToJson();
        return os;
    }

}// namespace AwsMock::Dto::SQS
