// Logger.hpp
// Spdlog wrapper with colored console output and convenience macros.
#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <string>

namespace Sports {

class Logger {
public:
    static void init();
    static void shutdown();
    static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_coreLogger; }

private:
    static inline std::shared_ptr<spdlog::logger> s_coreLogger;
};

}

// Logging Macros
#define LOG_TRACE(...)    ::Sports::Logger::getCoreLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)    ::Sports::Logger::getCoreLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)     ::Sports::Logger::getCoreLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Sports::Logger::getCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Sports::Logger::getCoreLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Sports::Logger::getCoreLogger()->critical(__VA_ARGS__)
