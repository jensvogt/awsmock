//
// Created by vogje01 on 03/06/2023.
//

#ifndef AWSMOCK_SERVICE_ABSTRACT_SERVER_H
#define AWSMOCK_SERVICE_ABSTRACT_SERVER_H

// C++ standard includes
#include <string>

// AwsMock includes
#include <awsmock/repository/ModuleDatabase.h>

namespace AwsMock::Service {

    /**
     * @brief Abstract HTTP request server
     *
     * @par
     * Base class for all server processes.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class AbstractServer {

      public:

        /**
         * @brief Constructor
         *
         * @param name manager name
         */
        explicit AbstractServer(std::string name);

        /**
         * Destructor
         */
        virtual ~AbstractServer() = default;

        /**
         * @brief Shutdown server
         */
        virtual void Shutdown();

      protected:

        /**
         * @brief Checks whether the module is active
         *
         * @param name module name
         * @return true if active
         */
        [[nodiscard]] bool IsActive(const std::string &name) const;

        /**
         * @brief Sets the running status in the module database
         */
        void SetRunning() const;

        /**
         * @brief Checks whether the module is running
         *
         * @return true if running
         */
        [[nodiscard]] bool IsRunning() const;

        /**
         * @brief Sets the running status in the module database
         */
        void SetStopped() const;

      private:

        /**
         * Service name
         */
        std::string _name;

        /**
         * Service database
         */
        Database::ModuleDatabase &_moduleDatabase;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_ABSTRACT_SERVER_H
