//
// Created by vogje01 on 5/12/24.
//

#ifndef AWSMOCK_CORE_TASK_POOL_H
#define AWSMOCK_CORE_TASK_POOL_H

// C++ includes
#include <map>
#include <string>
#include <thread>

// AwsMock includes
#include <awsmock/core/Task.h>
#include <awsmock/core/config/Configuration.h>

namespace AwsMock::Core {

    class TaskPool {

      public:

        /**
         * @brief Constructor
         */
        TaskPool();

        /**
         * @brief Singleton instance
         */
        static TaskPool &instance() {
            static TaskPool taskPool;
            return taskPool;
        }

        /**
         * @brief Adds a new thread
         *
         * @param name name of task
         * @param task task object
         */
        template<class N, class T>
        void Add(N &&name, T &&task) {
            _taskList[name] = &task;
            std::thread(&Task::DoWork, task).detach();
        }

      private:

        /**
         * Task list
         */
        std::map<std::string, const Core::Task *> _taskList;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_TASK_POOL_H
