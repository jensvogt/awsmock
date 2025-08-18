//
// Created by vogje01 on 30/05/2023.
//

#ifndef AWSMOCK_CORE_DTO_DELETE_OBJECTS_REQUEST_H
#define AWSMOCK_CORE_DTO_DELETE_OBJECTS_REQUEST_H

// C++ standard includes
#include <string>
#include <vector>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::S3 {

    /**
     * @brief Delete all objects of a bucket request.
     *
     * @par
     * The objects are identified by region and bucket name.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct DeleteObjectsRequest final : Common::BaseCounter<DeleteObjectsRequest> {

        /**
         * Bucket
         */
        std::string bucket;

        /**
         * Key
         */
        std::vector<std::string> keys;

        /**
         * Parse the delete objects request XML.
         *
         * @param xmlString notification XML string
         */
        void FromXml(const std::string &xmlString) {

            try {
                boost::property_tree::ptree pt;
                Core::XmlUtils::ReadXml(xmlString, &pt);
                for (auto &parent = pt.get_child("Delete"); const auto &[fst, snd]: parent) {
                    if (fst != "<xmlattr>") {
                        keys.emplace_back(snd.get_child("Key").data());
                    }
                }
            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            } catch (std::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend DeleteObjectsRequest tag_invoke(boost::json::value_to_tag<DeleteObjectsRequest>, boost::json::value const &v) {
            DeleteObjectsRequest r;
            r.bucket = Core::Json::GetStringValue(v, "bucket");
            if (Core::Json::AttributeExists(v, "keys")) {
                r.keys = boost::json::value_to<std::vector<std::string>>(v.at("keys"));
            }
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, DeleteObjectsRequest const &obj) {
            jv = {
                    {"region", obj.region},
                    {"user", obj.user},
                    {"requestId", obj.requestId},
                    {"bucket", obj.bucket},
                    {"keys", boost::json::value_from(obj.bucket)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_CORE_DTO_DELETE_OBJECTS_REQUEST_H
