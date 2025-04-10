//
// Created by vogje01 on 4/21/24.
//

#ifndef AWSMOCK_SERVICE_KMS_WORKER_H
#define AWSMOCK_SERVICE_KMS_WORKER_H

// C++ includes
#include <chrono>
#include <string>

// AwsMock includes
#include <awsmock/dto/kms/model/KeyState.h>
#include <awsmock/repository/KMSDatabase.h>

namespace AwsMock::Service {

    using std::chrono::system_clock;

    /**
     * @brief KMS worker thread
     *
     * @par
     * Used as background thread to do maintenance work. The KMS monitoring is 'self-starting', which means the constructor start
     * itself as a background process.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class KMSWorker {

      public:

        /**
         * @brief Constructor
         */
        explicit KMSWorker() = default;

        /**
         * @brief Delete keys which are pending for deletion
         */
        void DeleteKeys();

      private:

        /**
         * Database connection
         */
        Database::KMSDatabase &_kmsDatabase = Database::KMSDatabase::instance();
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_KMS_WORKER_H
