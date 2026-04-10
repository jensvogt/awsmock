//
// Created by vogje01 on 06/09/2023.
//

#include <awsmock/entity/s3/LifecycleConfiguration.h>

namespace AwsMock::Database::Entity::S3 {

    LifecycleConfiguration::LifecycleConfiguration(const view &mResult) {
        FromDocument(mResult);
    }

    view_or_value<view, value> LifecycleConfiguration::ToDocument() const {

        auto lifecycleConfigurationDoc = document{};
        lifecycleConfigurationDoc.append(kvp("id", id));
        lifecycleConfigurationDoc.append(kvp("prefix", prefix));
        lifecycleConfigurationDoc.append(kvp("status", LifeCycleStatusToString(status)));

        if (!transitions.empty()) {
            array transitionsArray;
            for (const auto &transition: transitions) {
                transitionsArray.append(transition.ToDocument());
            }
            lifecycleConfigurationDoc.append(kvp("transitions", transitionsArray));
        }
        return lifecycleConfigurationDoc.extract();
    }

    LifecycleConfiguration LifecycleConfiguration::FromDocument(const view &mResult) {

        LifecycleConfiguration l;
        l.id = Core::Bson::BsonUtils::GetStringValue(mResult["id"]);
        l.prefix = Core::Bson::BsonUtils::GetStringValue(mResult["prefix"]);
        l.status = LifeCycleStatusFromString(Core::Bson::BsonUtils::GetStringValue(mResult["status"]));

        // Transitions
        if (mResult.find("transitions") != mResult.end()) {
            l.transitions.clear();
            for (const bsoncxx::array::view transitionView{mResult["transitions"].get_array().value}; const bsoncxx::array::element &transitionElement: transitionView) {
                l.transitions.emplace_back(LifecycleTransition::FromDocument(transitionElement.get_document().view()));
            }
        }
        return l;
    }

} // namespace AwsMock::Database::Entity::S3
