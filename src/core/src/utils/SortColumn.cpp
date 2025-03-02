
#include <awsmock/core/SortColumn.h>

namespace AwsMock::Core {

    void SortColumn::FromDocument(const std::optional<view> &document) {

        // column = Bson::BsonUtils::GetStringValue(document, "column");
        // TODO: Fix me
        //sortDirection = Bson::BsonUtils::GetIntValue(document, "sortDirection");
    }

    view_or_value<view, value> SortColumn::ToDocument() const {

        try {
            document document;
            Bson::BsonUtils::SetStringValue(document, "column", column);
            Bson::BsonUtils::SetIntValue(document, "sortDirection", sortDirection);

            return document.extract();

        } catch (bsoncxx::exception &exc) {
            log_error << exc.what();
            throw JsonException(exc.what());
        }
    }

}// namespace AwsMock::Core