//
// Created by vogje01 on 4/28/24.
//

#include <awsmock/dto/s3/model/FilterRule.h>

namespace AwsMock::Dto::S3 {

    view_or_value<view, value> FilterRule::ToDocument() const {

        try {

            document document;
            Core::Bson::BsonUtils::SetStringValue(document, "name", NameTypeToString(name));
            Core::Bson::BsonUtils::SetStringValue(document, "value", filterValue);
            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void FilterRule::FromDocument(const view_or_value<view, value> &document) {

        try {

            name = NameTypeFromString(Core::Bson::BsonUtils::GetStringValue(document, "name"));
            filterValue = Core::Bson::BsonUtils::GetStringValue(document, "value");

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw Core::JsonException(exc.what());
        }
    }

    void FilterRule::FromXml(const boost::property_tree::ptree &pt) {
        if (pt.get_child_optional("S3Key")) {
            if (const boost::property_tree::ptree s3KeyNode = pt.get_child("S3Key"); s3KeyNode.get_child_optional("FilterRule")) {
                const boost::property_tree::ptree filterRuleNode = s3KeyNode.get_child("FilterRule");
                if (filterRuleNode.get_optional<std::string>("Name")) {
                    name = NameTypeFromString(filterRuleNode.get<std::string>("Name"));
                }
                if (filterRuleNode.get_optional<std::string>("Value")) {
                    filterValue = filterRuleNode.get<std::string>("Value");
                }
            }
        }
    }

}// namespace AwsMock::Dto::S3
