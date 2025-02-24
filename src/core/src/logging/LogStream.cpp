
#include <awsmock/core/LogStream.h>
#include <awsmock/core/config/Configuration.h>

namespace AwsMock::Core {

    long LogStream::logSize = DEFAULT_LOG_SIZE;
    int LogStream::logCount = DEFAULT_LOG_COUNT;

    void LogStream::Initialize() {
        plog::init<LogFormatter>(plog::error, plog::streamStdOut);
    }

    void LogStream::SetSeverity(const std::string &severity) {
        plog::get()->setMaxSeverity(plog::severityFromString(severity.c_str()));
    }

    void LogStream::SetFilename(const std::string &filename) {
        if (Configuration::instance().HasValue("awsmock.logging.file-size")) {
            logSize = Configuration::instance().GetValueLong("awsmock.logging.file-size");
        }
        if (Configuration::instance().HasValue("awsmock.logging.file-count")) {
            logCount = Configuration::instance().GetValueInt("awsmock.logging.file-count");
        }
        static plog::RollingFileAppender<LogFormatter> fileAppender(filename.c_str(), logSize, logCount);
        plog::get()->addAppender(&fileAppender);
        log_info << "Start logging to " << filename << " size: " << logSize << " count: " << logCount;
    }

}// namespace AwsMock::Core