//
// Created by vogje01 on 5/10/24.
//

#ifndef AWSMOCK_DTO_S3_OBJECT_VERSION_H
#define AWSMOCK_DTO_S3_OBJECT_VERSION_H

// C++ includes
#include <chrono>
#include <sstream>
#include <string>

// AwsMock includes
#include <awsmock/core/DateTimeUtils.h>
#include <awsmock/core/JsonUtils.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/XmlUtils.h>
#include <awsmock/core/exception/JsonException.h>
#include <awsmock/dto/s3/model/Owner.h>
#include <awsmock/dto/s3/model/RestoreStatus.h>

namespace AwsMock::Dto::S3 {

    using std::chrono::system_clock;

    struct ObjectVersion {

        /**
         * Key
         */
        std::string key;

        /**
         * ETag
         */
        std::string eTag;

        /**
         * VersionId
         */
        std::string versionId;

        /**
         * Storage class
         */
        std::string storageClass;

        /**
         * Checksum algorithm
         */
        std::vector<std::string> checksumAlgorithms;

        /**
         * Is latest
         */
        bool isLatest;

        /**
         * Size
         */
        long size;

        /**
         * Is latest
         */
        system_clock::time_point lastModified;

        /**
         * Owner
         */
        Owner owner;

        /**
         * Restore status
         */
        RestoreStatus restoreStatue;

        /**
         * Convert to a XML element
         *
         * @param pDoc XML document
         * @return XML element
         */
        [[nodiscard]] Poco::XML::AutoPtr<Poco::XML::Element> ToXmlElement(Poco::XML::AutoPtr<Poco::XML::Document> pDoc) const;

        /**
         * Convert to a JSON object
         *
         * @return JSON object
         */
        [[nodiscard]] Poco::JSON::Object ToJsonObject() const;

        /**
         * Convert to a JSON string
         *
         * @return JSON string
         */
        [[nodiscard]] std::string ToJson() const;

        /**
         * Converts the DTO to a string representation.
         *
         * @return DTO as string for logging.
         */
        [[nodiscard]] std::string ToString() const;

        /**
         * Stream provider.
         *
         * @return output stream
         */
        friend std::ostream &operator<<(std::ostream &os, const ObjectVersion &r);
    };

}// namespace AwsMock::Dto::S3

#endif// AWSMOCK_DTO_S3_OBJECT_VERSION_H
