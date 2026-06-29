
#include <awsmock/core/config/Configuration.h>
#include <awsmock/core/logging/LogStream.h>

#include <awsmock/core/logging/LoggingServer.h>
#include <ranges>

namespace {
    logger_t _logger{boost::log::keywords::channel = "Core"};
}

namespace Awsmock::Core {

    long LogStream::logSize = DEFAULT_LOG_SIZE;
    int LogStream::logCount = DEFAULT_LOG_COUNT;
    std::string LogStream::_logDir;
    std::string LogStream::_logPrefix;
    std::string LogStream::_currentLevel = "info";
    boost::log::trivial::severity_level LogStream::_severity;
    std::map<std::string, boost::log::trivial::severity_level> LogStream::_channelLevels;
    boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>> LogStream::console_sink;
    boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>> LogStream::file_sink;
    boost::shared_ptr<websocket::stream<beast::tcp_stream>> LogStream::_ws;
    boost::shared_ptr<LogWebsocketSink> LogStream::webSocketBackend(new LogWebsocketSink(_ws));
    boost::shared_ptr<webSocketSink_t> LogStream::web_socket_sink;
    boost::shared_ptr<boost::log::sinks::basic_sink_frontend> LogStream::logging_ws_sink;

    inline std::string processFuncName(const char *func) {
#if (defined(_WIN32) && !defined(__MINGW32__)) || defined(__OBJC__)
        return std::string(func);
#else
        const char *funcBegin = func;
        const char *funcEnd = strchr(funcBegin, '(');
        int foundTemplate = 0;

        if (!funcEnd) {
            return {func};
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

    void SetColorCoding(boost::log::record_view const &rec, boost::log::formatting_ostream &strm) {

        const auto severity = rec[boost::log::trivial::severity];
        if (severity) {
            switch (severity.get()) {
                case boost::log::trivial::trace:
                    strm << "\033[36m";
                    break;
                case boost::log::trivial::debug:
                    strm << "\033[32m";
                    break;
                case boost::log::trivial::info:
                    strm << "\033[97m";
                    break;
                case boost::log::trivial::warning:
                    strm << "\033[33m";
                    break;
                case boost::log::trivial::error:
                case boost::log::trivial::fatal:
                    strm << "\033[31m";
                    break;
                default:
                    break;
            }
        }
    }

    void ResetColorCoding(boost::log::formatting_ostream &strm) {
        strm << "\033[97m";
    }

    void LogColorFormatter(boost::log::record_view const &rec, boost::log::formatting_ostream &strm) {

        if (const auto raw = boost::log::extract<bool>("Raw", rec); raw && raw.get()) {
            strm << rec[boost::log::expressions::smessage];
            return;
        }

        std::string func = processFuncName(boost::log::extract<std::string>("Function", rec)->c_str());

#ifndef _WIN32
        SetColorCoding(rec, strm);
#endif

        auto date_time_formatter = boost::log::expressions::stream << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
        date_time_formatter(rec, strm);

        strm << " [" << rec[boost::log::trivial::severity] << "]";
        strm << " [" << rec[thread_id].get().native_id() << "]";
        if (const auto chan = boost::log::extract<std::string>("Channel", rec); chan) {
            strm << " [" << chan.get() << "]";
        }
        strm << " [" << func << ":" << boost::log::extract<int>("Line", rec) << "] ";

        strm << rec[boost::log::expressions::smessage];

#ifndef _WIN32
        ResetColorCoding(strm);
#endif
    }

    void LogFormatter(boost::log::record_view const &rec, boost::log::formatting_ostream &strm) {

        if (const auto raw = boost::log::extract<bool>("Raw", rec); raw && raw.get()) {
            strm << rec[boost::log::expressions::smessage];
            return;
        }

        std::string func = processFuncName(boost::log::extract<std::string>("Function", rec)->c_str());

        auto date_time_formatter = boost::log::expressions::stream << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
        date_time_formatter(rec, strm);

        strm << " [" << rec[boost::log::trivial::severity] << "]";
        strm << " [" << rec[thread_id].get().native_id() << "]";
        if (const auto chan = boost::log::extract<std::string>("Channel", rec); chan) {
            strm << " [" << chan.get() << "]";
        }
        strm << " [" << func << ":" << boost::log::extract<int>("Line", rec) << "] ";

        strm << rec[boost::log::expressions::smessage];
    }

    void LogStream::Initialize() {

        namespace net = boost::asio;

        if (Configuration::instance().getOr<bool>("awsmock.logging.console-active", true)) {
            boost::log::add_common_attributes();
            console_sink = boost::log::add_console_log(std::cout);
            console_sink->set_formatter(&LogColorFormatter);
        }
        // Apply the initial level so both the core filter and all sink filters are consistent.
        SetSeverity(_currentLevel);
    }

    std::string LogStream::GetSeverity() {
        return _currentLevel;
    }

    void LogStream::SetSeverity(const std::string &lvl) {
        if (!from_string(lvl.c_str(), lvl.length(), _severity)) {
            log_warning << "Unknown log level ignored: " << lvl;
            return;
        }
        _currentLevel = lvl;
        UpdateFilter();
    }

    void LogStream::SetChannelSeverity(const std::string &channel, const std::string &lvl) {
        boost::log::trivial::severity_level sev;
        if (!from_string(lvl.c_str(), lvl.length(), sev)) {
            log_warning << "Unknown log level ignored for channel " << channel << ": " << lvl;
            return;
        }
        _channelLevels[channel] = sev;
        UpdateFilter();
    }

    void LogStream::UpdateFilter() {
        auto channelLevels = _channelLevels;
        auto globalLevel = _severity;

        // Per-sink filters must be at least as permissive as the most verbose channel override.
        // The core filter runs first; a stale restrictive sink filter would silently drop records
        // the core correctly passed for a verbose channel.
        auto minLevel = globalLevel;
        for (const auto &lv: channelLevels | std::views::values) {
            if (lv < minLevel) minLevel = lv;
        }
        if (console_sink) console_sink->set_filter(boost::log::trivial::severity >= minLevel);
        if (file_sink) file_sink->set_filter(boost::log::trivial::severity >= minLevel);
        if (web_socket_sink) web_socket_sink->set_filter(boost::log::trivial::severity >= minLevel);
        if (logging_ws_sink) logging_ws_sink->set_filter(boost::log::trivial::severity >= minLevel);

        boost::log::core::get()->set_filter(
                [channelLevels, globalLevel](boost::log::attribute_value_set const &attrs) {
                    const auto sev = attrs["Severity"].extract<boost::log::trivial::severity_level>();
                    const auto chan = attrs["Channel"].extract<std::string>();
                    if (!sev) return false;
                    if (chan) {
                        if (const auto it = channelLevels.find(chan.get()); it != channelLevels.end()) {
                            return sev.get() >= it->second;
                        }
                    }
                    return sev.get() >= globalLevel;
                });
    }

    void LogStream::AddFile(const std::string &dir, const std::string &prefix, long size, int count) {
#ifdef _WIN32
        file_sink = add_file_log(
                boost::log::keywords::file_name = dir + "\\" + prefix + ".log ", boost::log::keywords::rotation_size = size,
                boost::log::keywords::target_file_name = dir + "\\" + prefix + "_ % N.log ", boost::log::keywords::format = &LogFormatter);
#else
        file_sink = add_file_log(
                boost::log::keywords::file_name = dir + "/" + prefix + ".log",
                boost::log::keywords::rotation_size = size,
                boost::log::keywords::target_file_name = dir + "/" + prefix + "_%N.log",
                boost::log::keywords::format = &LogColorFormatter);
#endif

        file_sink->locked_backend()->set_file_collector(boost::log::sinks::file::make_collector(
                boost::log::keywords::target = dir,
                boost::log::keywords::max_files = count));

        file_sink->locked_backend()->scan_for_files();
        UpdateFilter();

        log_info << "Start logging to file, dir: " << dir << ", prefix: " << prefix << " size: " << size << " count: " << count;
    }

    void LogStream::AddWebSocket(websocket::stream<beast::tcp_stream> &ws) {
        const boost::shared_ptr<boost::log::core> core = boost::log::core::get();
        webSocketBackend = boost::make_shared<LogWebsocketSink>(boost::make_shared<websocket::stream<beast::tcp_stream>>(std::move(ws)));
        web_socket_sink = boost::make_shared<webSocketSink_t>(webSocketBackend);
        web_socket_sink->set_formatter(&LogFormatter);
        core->add_sink(web_socket_sink);
        UpdateFilter();
    }

    void LogStream::AddLoggingWebSocket(boost::asio::io_context &ioc, unsigned int port) {
        auto mgr = std::make_shared<Service::Logging::WebSocketSessionManager>();

        // Start WebSocket Server in a background thread
        boost::thread([mgr, &ioc, port]() {
            RunLoggingWebSocketServer(ioc, port, mgr);
        }).detach();

        // Setup Boost.Log Sink
        auto backend = boost::make_shared<Service::Logging::WebSocketSinkBackend>(mgr);
        using sink_t = boost::log::sinks::synchronous_sink<Service::Logging::WebSocketSinkBackend>;
        const auto sink = boost::make_shared<sink_t>(backend);
        sink->set_formatter(&LogFormatter);
        logging_ws_sink = sink;

        boost::log::core::get()->add_sink(sink);
        UpdateFilter();
    }

    void LogStream::RemoveWebSocketSink() {
        const boost::shared_ptr<boost::log::core> core = boost::log::core::get();
        core->remove_sink(web_socket_sink);
    }

    void LogStream::RemoveConsoleLogs() {
        boost::log::core::get()->remove_sink(console_sink);
    }

    void LogStream::LogRaw(const std::string &message) {
        BOOST_LOG_SEV(_logger, boost::log::trivial::info)
                << boost::log::add_value("Raw", true)
                << message;
    }

}// namespace Awsmock::Core
