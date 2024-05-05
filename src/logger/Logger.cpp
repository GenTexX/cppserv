#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace cppserv {


    Ref<spdlog::logger> Logger::s_CoreLogger;
    Ref<spdlog::logger> Logger::s_FileLogger;

    void Logger::init() {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        s_CoreLogger = spdlog::stdout_color_mt("CPPSERV");
        s_CoreLogger->set_level(spdlog::level::trace);

        s_FileLogger = spdlog::rotating_logger_mt("FL_CPPSERV", "logs/cppserv.log", 1048576 * 5, 3);
        s_FileLogger->set_level(spdlog::level::trace);
    }

    Ref<spdlog::logger> Logger::GetCoreLogger() {
        return Logger::s_CoreLogger;
    }

    Ref<spdlog::logger> Logger::GetFileLogger() {
        return Logger::s_FileLogger;
    }

} // namespace cppserv
