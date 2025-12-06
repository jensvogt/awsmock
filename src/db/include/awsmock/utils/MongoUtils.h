//
// Created by vogje01 on 4/28/24.
//

#ifndef AWSMOCK_CORE_MONGO_UTILS_H
#define AWSMOCK_CORE_MONGO_UTILS_H

// Boost includes
#include <boost/date_time/posix_time/posix_time.hpp>

// Mongo included
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/pipeline.hpp>

namespace AwsMock::Database {

    using namespace boost::posix_time;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;
    using std::chrono::system_clock;

    class MongoUtils {

      public:

        /**
         * @brief Sets a time point or null.
         *
         * @param document MongoDB document
         * @param attribute attribute name
         * @param datetime date time value
         */
        static void SetDatetime(bsoncxx::builder::basic::document &document, const std::string &attribute, system_clock::time_point datetime);

        /**
         * @brief Return a chrono time_point
         *
         * @param result MongoDB result
         * @param attribute attribute name
         * @return time_point
         */
        static system_clock::time_point GetDatetime(const std::optional<bsoncxx::document::view> &result, const std::string &attribute);
    };

}// namespace AwsMock::Database

#endif// AWSMOCK_CORE_MONGO_UTILS_H
