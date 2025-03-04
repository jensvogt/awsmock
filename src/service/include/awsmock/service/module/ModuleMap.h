//
// Created by vogje01 on 10/6/24.
//

#ifndef AWSMOCK_SERVICE_MODULE_MAP_H
#define AWSMOCK_SERVICE_MODULE_MAP_H

// C++ includes
#include <map>
#include <string>

// Boost includes
#include <boost/asio/thread_pool.hpp>

// AwsMock includes
#include <awsmock/service/common/AbstractServer.h>

namespace AwsMock::Service {

    class ModuleMap {

      public:

        /**
         * @brief Constructor
         */
        ModuleMap();

        /**
         * @brief Singleton instance
         */
        static ModuleMap &instance() {
            static ModuleMap moduleMap;
            return moduleMap;
        }

        /**
         * @brief Returns a pointer to the module server
         *
         * @param name name of the module
         * @param server server pointer
         */
        void AddModule(const std::string &name, const std::shared_ptr<AbstractServer> &server);

        /**
         * @brief Returns a pointer to the module server
         *
         * @param name name of the module
         * @return server pointer
         */
        std::shared_ptr<AbstractServer> GetModule(const std::string &name);

        /**
         * @brief Checks the existence of a module server
         *
         * @param name name of the module
         * @return true if existing
         */
        bool HasModule(const std::string &name);

        /**
         * @brief Returns the module mpa
         *
         * @return full module map
         */
        std::map<std::string, std::shared_ptr<AbstractServer>> GetModuleMap();

      private:

        /**
         * Map
         */
        std::map<std::string, std::shared_ptr<AbstractServer>> moduleMap;
    };
}// namespace AwsMock::Service
#endif//AWSMOCK_SERVICE_MODULE_MAP_H
