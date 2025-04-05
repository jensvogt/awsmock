//
// Created by vogje01 on 10/7/24.
//

#ifndef AWSMOCK_CORE_PERIODIC_TASK_H
#define AWSMOCK_CORE_PERIODIC_TASK_H

// C++ includes
#include <functional>
#include <string>

// Boost includes
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/noncopyable.hpp>

// AwsMock includes
#include <awsmock/core/LogStream.h>

namespace AwsMock::Core {

    class PeriodicTask : boost::noncopyable {

      public:

        typedef std::function<void()> handler_fn;

        PeriodicTask(boost::asio::io_context &ioService, std::string const &name, int interval, handler_fn task, int delay);

        [[maybe_unused]] void execute(boost::system::error_code const &e);

        void start();

      private:

        [[maybe_unused]] void start_wait();

        boost::asio::io_context &ioService;
        boost::asio::deadline_timer timer;
        handler_fn task;
        std::string name;
        int interval;
        int _delay;
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_PERIODIC_TASK_H
