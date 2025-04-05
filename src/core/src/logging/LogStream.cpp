
#include <awsmock/core/LogStream.h>

namespace AwsMock::Core {

    long LogStream::logSize = DEFAULT_LOG_SIZE;
    int LogStream::logCount = DEFAULT_LOG_COUNT;
    boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>> LogStream::console_sink;
    boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>> LogStream::file_sink;

    inline std::string processFuncName(const char *func) {
#if (defined(_WIN32) && !defined(__MINGW32__)) || defined(__OBJC__)
        return std::string(func);
#else
        const char *funcBegin = func;
        const char *funcEnd = ::strchr(funcBegin, '(');
        int foundTemplate = 0;

        if (!funcEnd) {
            return std::string(func);
        }

        for (const char *i = funcEnd - 1; i >= funcBegin; --i)// search backwards for the first space char
        {
            if (*i == '>') {
                foundTemplate++;
            } else if (*i == '<') {
                foundTemplate--;
            } else if (*i == ' ' && foundTemplate == 0) {
                funcBegin = i + 1;
                break;
            }
        }
        auto f = std::string(funcBegin, funcEnd);
        if (const size_t position = f.find("::"); position != std::string::npos) {
            return f.substr(position + 2);
        }
        return f;

#endif
    }

    void LogFormatter(boost::log::record_view const &rec, boost::log::formatting_ostream &strm) {

        std::string func = processFuncName(boost::log::extract<std::string>("Function", rec)->c_str());

        auto date_time_formatter = boost::log::expressions::stream << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
        date_time_formatter(rec, strm);

        // The same for the severity
        strm << " [" << rec[boost::log::trivial::severity] << "]";
        strm << " [" << rec[process_id].get().native_id() << "]";
        strm << " [" << func << ":" << boost::log::extract<int>("Line", rec) << "] ";

        // Finally, put the record message to the stream
        strm << rec[boost::log::expressions::smessage];
    }

    void LogStream::Initialize() {

        console_sink = boost::log::add_console_log(std::cout);
        console_sink->set_formatter(&LogFormatter);
        console_sink->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);

        boost::log::add_common_attributes();
    }

    void LogStream::SetSeverity(const std::string &severity) {
        boost::log::trivial::severity_level lvl;
        from_string(severity.c_str(), severity.length(), lvl);
        console_sink->set_filter(boost::log::trivial::severity >= lvl);
        if (file_sink) {
            file_sink->set_filter(boost::log::trivial::severity >= lvl);
        }
        log_info << "Logging level set to " << severity;
    }

    void LogStream::SetFilename(const std::string &filename) {
        if (Configuration::instance().HasValue("awsmock.logging.file-size")) {
            logSize = Configuration::instance().GetValueLong("awsmock.logging.file-size");
        }
        if (Configuration::instance().HasValue("awsmock.logging.file-count")) {
            logCount = Configuration::instance().GetValueInt("awsmock.logging.file-count");
        }
        file_sink = add_file_log(
                boost::log::keywords::file_name = "awsmock-%N.log",
                boost::log::keywords::rotation_size = logSize,
                boost::log::keywords::max_files = logCount,
                boost::log::keywords::format = &LogFormatter);

        log_info << "Start logging to " << filename << " size: " << logSize << " count: " << logCount;
    }

}// namespace AwsMock::Core