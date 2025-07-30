//
// Created by vogje01 on 12/18/23.
//

#ifndef AWSMOCK_DTO_TRANSFER_USER_H
#define AWSMOCK_DTO_TRANSFER_USER_H

// C++ includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/logging/LogStream.h>
#include <awsmock/dto/common/BaseCounter.h>

namespace AwsMock::Dto::Transfer {

    /**
     * @brief Transfer server user
     *
     * Example
     * @code{.json}
     * @endcode
     * {
     *   "Arn": "arn:aws:transfer:us-east-1:176354371281:user/s-01234567890abcdef/charlie",
     *   "HomeDirectory": "/data/home/charlie",
     *   "SshPublicKeyCount": 1,
     *   "Role": "arn:aws:iam::176354371281:role/transfer-role1",
     *   "Tags": [
     *      {
     *         "Key": "Name",
     *         "Value": "user1"
     *      }
     *   ],
     *   "UserName": "my_user"
     * }
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct User final : Common::BaseCounter<User> {

        /**
         * User name
         */
        std::string userName;

        /**
         * ARN
         */
        std::string arn;

        /**
         * Role
         */
        std::string role;

        /**
         * Ssh public key count
         */
        long sshPublicKeyCount = 0;

        /**
         * Home directory
         */
        std::string homeDirectory;

        /**
         * Password
         */
        std::string password;

        /**
         * @brief Converts the DTO to a JSON representation.
         *
         * @return DTO as string
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Arn", arn);
                Core::Bson::BsonUtils::SetStringValue(document, "Role", role);
                Core::Bson::BsonUtils::SetStringValue(document, "UserName", userName);
                Core::Bson::BsonUtils::SetStringValue(document, "HomeDirectory", homeDirectory);
                Core::Bson::BsonUtils::SetStringValue(document, "Password", password);
                Core::Bson::BsonUtils::SetIntValue(document, "SshPublicKeyCount", sshPublicKeyCount);
                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts a JSON string to a user
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                region = Core::Bson::BsonUtils::GetStringValue(document, "Region");
                arn = Core::Bson::BsonUtils::GetStringValue(document, "Arn");
                role = Core::Bson::BsonUtils::GetStringValue(document, "Role");
                userName = Core::Bson::BsonUtils::GetStringValue(document, "UserName");
                password = Core::Bson::BsonUtils::GetStringValue(document, "Password");
                homeDirectory = Core::Bson::BsonUtils::GetStringValue(document, "HomeDirectory");
                sshPublicKeyCount = Core::Bson::BsonUtils::GetIntValue(document, "SshPublicKeyCount");

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend User tag_invoke(boost::json::value_to_tag<User>, boost::json::value const &v) {
            User r;
            r.arn = Core::Json::GetStringValue(v, "Arn");
            r.userName = Core::Json::GetStringValue(v, "UserName");
            r.role = Core::Json::GetStringValue(v, "Role");
            r.sshPublicKeyCount = Core::Json::GetLongValue(v, "SshPublicKeyCount");
            r.homeDirectory = Core::Json::GetStringValue(v, "HomeDirectory");
            r.password = Core::Json::GetStringValue(v, "Password");
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, User const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Arn", obj.arn},
                    {"UserName", obj.userName},
                    {"Role", obj.role},
                    {"SshPublicKeyCount", obj.sshPublicKeyCount},
                    {"HomeDirectory", obj.homeDirectory},
                    {"Password", obj.password},
            };
        }
    };

    typedef std::vector<User> UserList;

}// namespace AwsMock::Dto::Transfer

#endif// AWSMOCK_DTO_TRANSFER_USER_H
