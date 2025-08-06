//
// Created by vogje01 on 8/5/25.
//

#ifndef AWSMOCK_CORE_SEMAPHORE_H
#define AWSMOCK_CORE_SEMAPHORE_H

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/lock_types.hpp>
#include <boost/thread/mutex.hpp>

namespace AwsMock::Core {

    class Semaphore {

        //The current semaphore count.
        unsigned int _count{};

        // Any code that reads or writes the _count data must hold a lock on the mutex.
        boost::mutex _mutex;

        // Code that increments _count must notify the condition variable.
        boost::condition_variable _condition;

      public:

        explicit Semaphore(const unsigned int initial_count) : _count(initial_count) {
        }

        unsigned int GetCount()//for debugging/testing only
        {
            // The "lock" object locks the mutex when it's constructed
            // and unlocks it when it's destroyed.
            boost::unique_lock lock(_mutex);
            return _count;
        }

        void Release() {
            boost::unique_lock lock(_mutex);

            ++_count;

            // Wake up any waiting threads. Always do this, even if count_ wasn't 0 on entry. Otherwise, we might not
            // wake up enough waiting threads if we get a number of signal() calls in a row.
            _condition.notify_one();
        }

        void Acquire() {
            boost::unique_lock lock(_mutex);
            while (_count == 0) {
                _condition.wait(lock);
            }
            --_count;
        }
    };

}// namespace AwsMock::Core

#endif// AWSMOCK_CORE_SEMAPHORE_H
