//
// Created by vogje01 on 4/11/24.
//

#ifndef AWSMOCK_DTO_SECRETSMANAGER_SECRET_H
#define AWSMOCK_DTO_SECRETSMANAGER_SECRET_H


// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/core/BsonUtils.h>
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/secretsmanager/model/RotationRules.h>

namespace AwsMock::Dto::SecretsManager {

    /**
     * @brief Secrets list filter
     *
     * Example:
     * @code{.json}
     *  {
     *     "Name": "string",
     *     "OwningService": "string",
     *     "ARN": "string",
     *     "Description": "string",
     *     "KmsKeyId": "string",
     *     "PrimaryRegion": "string",
     *     "CreatedDate": number,
     *     "DeletedDate": number,
     *     "LastAccessedDate": number,
     *     "LastChangedDate": number,
     *     "LastRotatedDate": number,
     *     "NextRotationDate": number,
     *     "RotationEnabled": boolean,
     *     "RotationLambdaARN": "string",
     *     "RotationRules": {
     *        "AutomaticallyAfterDays": number,
     *        "Duration": "string",
     *        "ScheduleExpression": "string"
     *     },
     *     "SecretVersionsToStages": {
     *        "string" : [ "string" ]
     *     },
     *     "Tags": [
     *       {
     *          "Key": "string",
     *          "Value": "string"
     *       }
     *    ]
     * }
     * @endcode
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct Secret final : Common::BaseCounter<Secret> {

        /**
         * AWS ARN
         */
        std::string arn;

        /**
         * Name
         */
        std::string name;

        /**
         * Description
         */
        std::string description;

        /**
         * KMS key ID
         */
        std::string kmsKeyId;

        /**
         * Secret owner
         */
        std::string owningService;

        /**
         * Primary region
         */
        std::string primaryRegion;

        /**
         * Creation date
         */
        long createdDate = 0;

        /**
         * Deleted date
         */
        long deletedDate = 0;

        /**
         * Last access date
         */
        long lastAccessedDate = 0;

        /**
         * Last changed date
         */
        long lastChangedDate = 0;

        /**
         * Last rotation date
         */
        long lastRotatedDate = 0;

        /**
         * Next rotation date
         */
        long nextRotatedDate = 0;

        /**
         * Next rotation date
         */
        bool rotationEnabled = false;

        /**
         * Rotation lambda ARN
         */
        std::string rotationLambdaARN;

        /**
         * Rotation rules
         */
        RotationRules rotationRules;

        /**
         * @brief Converts the DTO to a JSON object.
         *
         * @return DTO as JSON object.
         */
        [[nodiscard]] view_or_value<view, value> ToDocument() const {

            try {

                document document;
                Core::Bson::BsonUtils::SetStringValue(document, "Name", name);
                Core::Bson::BsonUtils::SetStringValue(document, "ARN", arn);
                Core::Bson::BsonUtils::SetStringValue(document, "Description", description);
                Core::Bson::BsonUtils::SetStringValue(document, "KmsKeyId", kmsKeyId);
                Core::Bson::BsonUtils::SetStringValue(document, "OwningService", owningService);
                Core::Bson::BsonUtils::SetStringValue(document, "PrimaryRegion", primaryRegion);
                Core::Bson::BsonUtils::SetLongValue(document, "CreatedDate", createdDate);
                Core::Bson::BsonUtils::SetLongValue(document, "DeletedDate", deletedDate);
                Core::Bson::BsonUtils::SetLongValue(document, "LastAccessedDate", lastAccessedDate);
                Core::Bson::BsonUtils::SetLongValue(document, "LastChangedDate", lastChangedDate);
                Core::Bson::BsonUtils::SetLongValue(document, "LastRotatedDate", lastRotatedDate);
                Core::Bson::BsonUtils::SetLongValue(document, "NextRotatedDate", nextRotatedDate);
                Core::Bson::BsonUtils::SetBoolValue(document, "RotationEnabled", rotationEnabled);
                Core::Bson::BsonUtils::SetStringValue(document, "RotationLambdaARN", rotationLambdaARN);

                document.append(kvp("RotationRules", rotationRules.ToDocument()));

                return document.extract();

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

        /**
         * @brief Converts the JSON object to DTO.
         *
         * @param document JSON object
         */
        void FromDocument(const view_or_value<view, value> &document) {

            try {

                name = Core::Bson::BsonUtils::GetStringValue(document, "Name");
                arn = Core::Bson::BsonUtils::GetStringValue(document, "ARN");
                description = Core::Bson::BsonUtils::GetStringValue(document, "Description");
                kmsKeyId = Core::Bson::BsonUtils::GetStringValue(document, "KmsKeyId");
                owningService = Core::Bson::BsonUtils::GetStringValue(document, "OwningService");
                primaryRegion = Core::Bson::BsonUtils::GetStringValue(document, "PrimaryRegion");
                createdDate = Core::Bson::BsonUtils::GetLongValue(document, "CreatedDate");
                deletedDate = Core::Bson::BsonUtils::GetLongValue(document, "DeletedDate");
                lastAccessedDate = Core::Bson::BsonUtils::GetLongValue(document, "LastAccessedDate");
                lastChangedDate = Core::Bson::BsonUtils::GetLongValue(document, "LastChangedDate");
                lastRotatedDate = Core::Bson::BsonUtils::GetLongValue(document, "LastRotatedDate");
                nextRotatedDate = Core::Bson::BsonUtils::GetLongValue(document, "NextRotatedDate");
                rotationEnabled = Core::Bson::BsonUtils::GetBoolValue(document, "RotationEnabled");
                rotationLambdaARN = Core::Bson::BsonUtils::GetStringValue(document, "RotationLambdaARN");

                if (document.view().find("RotationRules") != document.view().end()) {
                    rotationRules.FromDocument(document.view()["RotationRules"].get_document().value);
                }

            } catch (bsoncxx::exception &exc) {
                log_error << exc.what();
                throw Core::JsonException(exc.what());
            }
        }

      private:

        friend Secret tag_invoke(boost::json::value_to_tag<Secret>, boost::json::value const &v) {
            Secret r;
            r.name = Core::Json::GetStringValue(v, "Name");
            r.arn = Core::Json::GetStringValue(v, "ARN");
            r.description = Core::Json::GetStringValue(v, "Description");
            r.kmsKeyId = Core::Json::GetStringValue(v, "KmsKeyId");
            r.owningService = Core::Json::GetStringValue(v, "OwningService");
            r.primaryRegion = Core::Json::GetStringValue(v, "PrimaryRegion");
            r.createdDate = Core::Json::GetLongValue(v, "CreatedDate");
            r.deletedDate = Core::Json::GetLongValue(v, "DeletedDate");
            r.lastAccessedDate = Core::Json::GetLongValue(v, "LastAccessedDate");
            r.lastChangedDate = Core::Json::GetLongValue(v, "LastChangedDate");
            r.lastRotatedDate = Core::Json::GetLongValue(v, "LastRotatedDate");
            r.nextRotatedDate = Core::Json::GetLongValue(v, "NextRotatedDate");
            r.rotationEnabled = Core::Json::GetLongValue(v, "RotationEnabled");
            r.rotationLambdaARN = Core::Json::GetLongValue(v, "RotationLambdaARN");
            r.rotationRules = boost::json::value_to<RotationRules>(v.at("NextRotatedDate"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Secret const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Name", obj.name},
                    {"ARN", obj.arn},
                    {"Description", obj.description},
                    {"KmsKeyId", obj.kmsKeyId},
                    {"OwningService", obj.owningService},
                    {"PrimaryRegion", obj.primaryRegion},
                    {"CreatedDate", obj.createdDate},
                    {"DeletedDate", obj.deletedDate},
                    {"LastAccessedDate", obj.lastAccessedDate},
                    {"LastChangedDate", obj.lastChangedDate},
                    {"LastRotatedDate", obj.lastRotatedDate},
                    {"BextRotatedDate", obj.nextRotatedDate},
                    {"RotationEnabled", obj.rotationEnabled},
                    {"RotationLambdaARN", obj.rotationLambdaARN},
                    {"RotationRules", boost::json::value_from(obj.rotationRules)},
            };
        }
    };

}// namespace AwsMock::Dto::SecretsManager

#endif//AWSMOCK_DTO_SECRETSMANAGER_SECRET_H
