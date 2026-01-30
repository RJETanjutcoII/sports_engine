// Logger.cpp
// Initializes spdlog with colored console sink.
#include "Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <vector>

namespace Sports {

void Logger::init() {
    std::vector<spdlog::sink_ptr> sinks;

    // Console sink with colors
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("%^[%T] [%l] %v%$");
    sinks.push_back(consoleSink);

    s_coreLogger = std::make_shared<spdlog::logger>("SPORTS", sinks.begin(), sinks.end());
    spdlog::register_logger(s_coreLogger);
    s_coreLogger->set_level(spdlog::level::trace);
    s_coreLogger->flush_on(spdlog::level::trace);

    LOG_INFO("Logger initialized");
}

void Logger::shutdown() {
    LOG_INFO("Logger shutting down");
    spdlog::shutdown();
}

}
