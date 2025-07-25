#pragma once

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

namespace ormcxx
{
    enum class LogLevel
    {
        Debug = 0,
        Info,
        Warning,
        Error,
        Critical,
    };

    class ILogHandler
    {
    public:
        virtual ~ILogHandler() = default;
        virtual void log(const std::string& message, LogLevel level) = 0;
    };

    class CerrLogHandler : public ILogHandler
    {
    public:
        void log(const std::string& message, LogLevel level) override;

    private:
        static std::string timestamp();
    };

    class logger
    {
    public:
        logger(LogLevel level);
        ~logger();

        //
        template<typename T>
        logger& operator<<(T const& value)
        {
            if (level_ >= get_current_log_level())
            {
                stringstream_ << value;
            }
            return *this;
        }

        //
        static void setLogLevel(LogLevel level);

        static void setHandler(ILogHandler* handler);
        
        static LogLevel get_current_log_level();
    
    private:
        //
        static LogLevel& get_log_level_ref();
        static ILogHandler*& get_handler_ref();
        //
        std::ostringstream stringstream_;
        LogLevel level_;
    };
} // namespace ormcxx

#define ORMCXX_LOG_CRITICAL                                                  \
    if (ormcxx::logger::get_current_log_level() <= ormcxx::LogLevel::Critical) \
    ormcxx::logger(ormcxx::LogLevel::Critical)
#define ORMCXX_LOG_ERROR                                                  \
    if (ormcxx::logger::get_current_log_level() <= ormcxx::LogLevel::Error) \
    ormcxx::logger(ormcxx::LogLevel::Error)
#define ORMCXX_LOG_WARNING                                                  \
    if (ormcxx::logger::get_current_log_level() <= ormcxx::LogLevel::Warning) \
    ormcxx::logger(ormcxx::LogLevel::Warning)
#define ORMCXX_LOG_INFO                                                  \
    if (ormcxx::logger::get_current_log_level() <= ormcxx::LogLevel::Info) \
    ormcxx::logger(ormcxx::LogLevel::Info)
#define ORMCXX_LOG_DEBUG                                                  \
    if (ormcxx::logger::get_current_log_level() <= ormcxx::LogLevel::Debug) \
    ormcxx::logger(ormcxx::LogLevel::Debug)
