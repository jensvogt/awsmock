//
// Created by vogje01 on 5/12/24.
//

#ifndef AWSMOCK_SERVICE_S3_HASH_CREATOR_H
#define AWSMOCK_SERVICE_S3_HASH_CREATOR_H

// AwsMock includes
#include <awsmock/core/CryptoUtils.h>
#include <awsmock/entity/s3/Object.h>
#include <awsmock/repository/S3Database.h>

#define DEFAULT_DATA_DIR "/home/awsmock/data"

namespace AwsMock::Service {

    /**
     * @brief Asynchronous hash creator (MD5, SHA1, SHA256)
     *
     * Depending on the cryptographic algorithm used, the hashing of a file or string can take some time. Therefore, the generation of the
     * cryptographic hash is processed asynchronously. Can process several hashes sequentially.
     *
     * @code{.cpp}
     * if (!request.checksumAlgorithm.empty()) {
     *   Core::TaskPool::instance().Add<std::string, S3HashCreator>("s3-hashing", S3HashCreator({MD5, SHA1, SHA256}, object));
     *   log_debug << "Checksums, bucket: " << request.bucket << " key: " << request.key << " sha1: " << object.sha1sum << " sha256: " << object.sha256sum;
     * }
     * @endcode
     * will generate MD5, SHA1 and SHA256 hashes of the object.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class S3HashCreator {

      public:

        /**
         * @brief Constructor
         */
        explicit S3HashCreator() = default;

        /**
         * @brief Work method
         *
         * Generated several hashes for the S3 object (SHA1, SHA256, etc.)
         *
         * @param algorithms vector of algorithm names
         * @param object S3 object to hash
         */
        void operator()(std::vector<std::string> &algorithms, Database::Entity::S3::Object &object) const;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_S3_HASH_CREATOR_H
