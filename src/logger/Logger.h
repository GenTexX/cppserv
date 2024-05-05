#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "../core/cppservcore.h"

#include <spdlog/spdlog.h>

namespace cppserv
{
    

    class Logger {
    public:
        static void init();

        static Ref<spdlog::logger> GetCoreLogger();
        static Ref<spdlog::logger> GetFileLogger();

    private:
        static Ref<spdlog::logger> s_CoreLogger;
        static Ref<spdlog::logger> s_FileLogger;

    };


} // namespace cppserv


#endif // __LOGGER_H__

#define CPPSERV_TRACE(...) cppserv::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define CPPSERV_INFO(...) cppserv::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define CPPSERV_WARN(...) cppserv::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define CPPSERV_ERROR(...) cppserv::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define CPPSERV_CRITICAL(...) cppserv::Logger::GetCoreLogger()->critical(__VA_ARGS__)
#define CPPSERV_LOG(...) cppserv::Logger::GetFileLogger()->trace(__VA_ARGS__)