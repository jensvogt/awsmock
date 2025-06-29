//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_RESPONSE_H
#define AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_RESPONSE_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/StringUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/exception/ServiceException.h>
#include <awsmock/dto/sqs/model/BatchResultErrorEntry.h>
#include <awsmock/dto/sqs/model/DeleteMessageBatchResultEntry.h>

namespace AwsMock::Dto::SQS {

    struct DeleteMessageBatchResponse final : Common::BaseCounter<DeleteMessageBatchResponse> {

        /**
         * Resource
         */
        std::string resource = "SQS";

        /**
         * Failed
         */
        std::vector<BatchResultErrorEntry> failed;

        /**
         * Successful
         */
        std::vector<DeleteMessageBatchResultEntry> successfull;

        /**
         * Convert to XML representation
         *
         * @return XML string
         */
        [[nodiscard]] std::string ToXml() const {

            try {

                boost::property_tree::ptree pt;

                // Failed
                if (failed.empty()) {
                    boost::property_tree::ptree failedArray;
                    for (const auto &f: failed) {
                        boost::property_tree::ptree element;
                        element.put("Id", f.id);
                        element.put("Code", f.code);
                        element.put("SenderFault", f.senderFault);
                        element.put("Message", f.message);
                        failedArray.push_back(std::make_pair("", element));
                    }
                    pt.add_child("DeleteMessageBatchResponse.DeleteMessageBatchResult.Failed", failedArray);
                }

                // Successful
                if (successfull.empty()) {
                    boost::property_tree::ptree successArray;
                    for (const auto &s: successfull) {
                        boost::property_tree::ptree element;
                        element.put("Id", s.id);
                        successArray.push_back(std::make_pair("", element));
                    }
                    pt.add_child("DeleteMessageBatchResponse.DeleteMessageBatchResult.Successful", successArray);
                }

                // Metadata
                pt.put("DeleteMessageBatchResponse.ResponseMetadata.RequestId", requestId);
                return Core::XmlUtils::ToXmlString(pt);

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend DeleteMessageBatchResponse tag_invoke(boost::json::value_to_tag<DeleteMessageBatchResponse>, boost::json::value const &v) {
            DeleteMessageBatchResponse r;
            r.resource = Core::Json::GetStringValue(v, "Resource");
            if (Core::Json::AttributeExists(v, "Successful")) {
                r.successfull = boost::json::value_to<std::vector<DeleteMessageBatchResultEntry>>(v.at("Successful"));
            }
            if (Core::Json::AttributeExists(v, "Failed")) {
                r.failed = boost::json::value_to<std::vector<BatchResultErrorEntry>>(v.at("Failed"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteMessageBatchResponse const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Resource", obj.resource},
                    {"Successful", boost::json::value_from(obj.successfull)},
                    {"Failed", boost::json::value_from(obj.failed)},
            };
        }
    };

}// namespace AwsMock::Dto::SQS

#endif// AWSMOCK_DTO_SQS_DELETE_MESSAGE_BATCH_RESPONSE_H
