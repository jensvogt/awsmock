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

    void LifecycleConfiguration::FromDocument(const view &mResult) {

        id = Core::Bson::BsonUtils::GetStringValue(mResult["id"]);
        prefix = Core::Bson::BsonUtils::GetStringValue(mResult["prefix"]);
        status = LifeCycleStatusFromString(Core::Bson::BsonUtils::GetStringValue(mResult["status"]));

        // Transitions
        if (mResult.find("transitions") != mResult.end()) {
            transitions.clear();
            for (const bsoncxx::array::view transitionView{mResult["transitions"].get_array().value}; const bsoncxx::array::element &transitionElement: transitionView) {
                LifecycleTransition transition;
                transition.FromDocument(transitionElement.get_document().view());
                transitions.emplace_back(transition);
            }
        }
    }

}// namespace AwsMock::Database::Entity::S3
