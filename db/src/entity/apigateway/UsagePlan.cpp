//
// Created by vogje01 on 07/08/2026
//

#include <awsmock/entity/apigateway/UsagePlan.h>

namespace Awsmock::Database::Entity::ApiGateway {

    view_or_value<view, value> UsagePlanQuota::ToDocument() const {
        document doc;
        doc.append(kvp("limit", bsoncxx::types::b_int64(limit)));
        doc.append(kvp("offset", bsoncxx::types::b_int64(offset)));
        doc.append(kvp("period", period));
        return doc.extract();
    }

    void UsagePlanQuota::FromDocument(const std::optional<view> &doc) {
        limit = Core::Bson::BsonUtils::GetLongValue(doc, "limit");
        offset = Core::Bson::BsonUtils::GetLongValue(doc, "offset");
        period = Core::Bson::BsonUtils::GetStringValue(doc, "period");
    }

    view_or_value<view, value> UsagePlanThrottle::ToDocument() const {
        document doc;
        doc.append(kvp("burstLimit", bsoncxx::types::b_int64(burstLimit)));
        doc.append(kvp("rateLimit", rateLimit));
        return doc.extract();
    }

    void UsagePlanThrottle::FromDocument(const std::optional<view> &doc) {
        burstLimit = Core::Bson::BsonUtils::GetLongValue(doc, "burstLimit");
        rateLimit = Core::Bson::BsonUtils::GetDoubleValue(doc, "rateLimit");
    }

    view_or_value<view, value> UsagePlan::ToDocument() const {

        document planDocument;
        planDocument.append(kvp("region", region));
        planDocument.append(kvp("id", id));
        planDocument.append(kvp("name", name));
        planDocument.append(kvp("description", description));
        planDocument.append(kvp("quota", quota.ToDocument()));
        planDocument.append(kvp("throttle", throttle.ToDocument()));
        planDocument.append(kvp("created", bsoncxx::types::b_date(created)));
        planDocument.append(kvp("modified", bsoncxx::types::b_date(modified)));

        if (!tags.empty()) {
            document tagsDoc;
            for (const auto &[k, v]: tags) {
                tagsDoc.append(kvp(k, v));
            }
            planDocument.append(kvp("tags", tagsDoc));
        }
        return planDocument.extract();
    }

    void UsagePlan::FromDocument(const std::optional<view> &mResult) {

        oid = Core::Bson::BsonUtils::GetOidValue(mResult, "_id");
        region = Core::Bson::BsonUtils::GetStringValue(mResult, "region");
        id = Core::Bson::BsonUtils::GetStringValue(mResult, "id");
        name = Core::Bson::BsonUtils::GetStringValue(mResult, "name");
        description = Core::Bson::BsonUtils::GetStringValue(mResult, "description");
        created = Core::Bson::BsonUtils::GetDateValue(mResult, "created");
        modified = Core::Bson::BsonUtils::GetDateValue(mResult, "modified");

        if (mResult.value().find("quota") != mResult.value().end()) {
            const view quotaView = mResult.value()["quota"].get_document().value;
            const std::optional<view> quotaOpt = quotaView;
            quota.FromDocument(quotaOpt);
        }

        if (mResult.value().find("throttle") != mResult.value().end()) {
            const view throttleView = mResult.value()["throttle"].get_document().value;
            const std::optional<view> throttleOpt = throttleView;
            throttle.FromDocument(throttleOpt);
        }

        if (mResult.value().find("tags") != mResult.value().end()) {
            tags.clear();
            for (const view tagsObject = mResult.value()["tags"].get_document().value; const auto &t: tagsObject) {
                const std::string key = bsoncxx::string::to_string(t.key());
                const std::string val = bsoncxx::string::to_string(tagsObject[key].get_string().value);
                tags[key] = val;
            }
        }
    }

}// namespace Awsmock::Database::Entity::ApiGateway
