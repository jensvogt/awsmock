//
// Created by vogje01 on 2/16/26.
//

#ifndef AWSMOCK_CORE_EVENTBUS_H
#define AWSMOCK_CORE_EVENTBUS_H

// Boost includes
#include <boost/signals2/signal.hpp>

namespace AwsMock::Core {

    /**
     * @brief General event bus using boost signals2
     *
     * @author jens.vogt\@opitz-consulting.com
     * @since 1.14.7
     */
    class EventBus {
      public:

        /**
         * @brief Singleton instance
         */
        static EventBus &instance() {
            static EventBus eventBus;
            return eventBus;
        }

        /**
         * @brief Signal for an FTP server file upload.
         *
         * @par
         * The template parameter are containing the filename, the username who uploaded the file and the file metadata. Metadata
         * is a string map, which contains usually the user-agent and user-agent-id.
         *
         * @tparam username
         * @tparam filename
         * @tparam metadata
         */
        boost::signals2::signal<void(std::string, std::string, std::map<std::string, std::string>)> sigFtpUpload;
    };
};// namespace AwsMock::Core

#endif//AWSMOCK_CORE_EVENTBUS_H
