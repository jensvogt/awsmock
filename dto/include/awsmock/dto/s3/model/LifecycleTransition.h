//
// Created by vogje01 on 1/9/26.
//

#ifndef AWSMOCK_DTO_S3_LIFECYCLE_TRANSITION_H
#define AWSMOCK_DTO_S3_LIFECYCLE_TRANSITION_H

// Boost includes
#include <boost/property_tree/ptree.hpp>

// AwsMock includes
#include <awsmock/dto/common/BaseCounter.h>
#include <awsmock/dto/s3/model/StorageClass.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    /**
     * @brief Filter rule for the S3 bucket notification to SQS queues
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    struct LifecycleTransition final : Common::BaseCounter<LifecycleTransition> {

        /**
         * @brief Fixed timestamp
         */
        system_clock::time_point date;

        /**
         * @brief Number of days
         */
        int days{};

        /**
         * @brief Storage class
         */
        StorageClass storageClass = StorageClass::STANDARD;

        /**
         * @brief Convert from an XML string
         *
         * @param pt boost a property tree
         */
        void FromXml(const boost::property_tree::ptree &pt) {
            date = Core::DateTimeUtils::FromISO8601(pt.get<std::string>("Date", ""));
            days = pt.get<int>("Days", 0);
            storageClass = StorageClassFromString(pt.get<std::string>("StorageClass", "STANDARD"));
        }

      private:

        friend LifecycleTransition tag_invoke(boost::json::value_to_tag<LifecycleTransition>, boost::json::value const &v) {
            LifecycleTransition r;
            r.date = Core::Json::GetDatetimeValue(v, "Date");
            r.days = Core::Json::GetIntValue(v, "Days");
            r.storageClass = StorageClassFromString(Core::Json::GetStringValue(v, "StorageClass"));
            return r;
        }

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, LifecycleTransition const &obj) {
            jv = {
                    {"Region", obj.region},
                    {"User", obj.user},
                    {"RequestId", obj.requestId},
                    {"Date", Core::DateTimeUtils::ToISO8601(obj.date)},
                    {"Days", obj.days},
                    {"StorageClass", StorageClassToString(obj.storageClass)},
            };
        }
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_LIFECYCLE_TRANSITION_H
