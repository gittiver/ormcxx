#include "ormcxx/logging.hpp"

namespace ormcxx
{

	void CerrLogHandler::log(const std::string& message, LogLevel level)
	{
		std::string prefix;
		switch (level)
		{
		case LogLevel::Debug:
			prefix = "DEBUG   ";
			break;
		case LogLevel::Info:
			prefix = "INFO    ";
			break;
		case LogLevel::Warning:
			prefix = "WARNING ";
			break;
		case LogLevel::Error:
			prefix = "ERROR   ";
			break;
		case LogLevel::Critical:
			prefix = "CRITICAL";
			break;
		}
		std::cerr << std::string("(") + timestamp() + std::string(") [") + prefix + std::string("] ") + message << std::endl;
	}

	std::string CerrLogHandler::timestamp()
	{
		char date[32];
		time_t t = time(0);

		tm my_tm;

#if defined(_MSC_VER) || defined(__MINGW32__)
#ifdef ORMCXX_USE_LOCALTIMEZONE
		localtime_s(&my_tm, &t);
#else
		gmtime_s(&my_tm, &t);
#endif
#else
#ifdef ORMCXX_USE_LOCALTIMEZONE
		localtime_r(&t, &my_tm);
#else
		gmtime_r(&t, &my_tm);
#endif
#endif

		size_t sz = strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", &my_tm);
		return std::string(date, date + sz);
	}

	logger::logger(LogLevel level) :
		level_(level)
	{
	}
	logger::~logger()
	{
		if (level_ >= get_current_log_level())
		{
			get_handler_ref()->log(stringstream_.str(), level_);
		}
	}


	void logger::setLogLevel(LogLevel level) { get_log_level_ref() = level; }
	void logger::setHandler(ILogHandler* handler) { get_handler_ref() = handler; }

	LogLevel logger::get_current_log_level() { return get_log_level_ref(); }

	//
	LogLevel& logger::get_log_level_ref()
	{
		static LogLevel current_level = static_cast<LogLevel>(LogLevel::Debug);
		return current_level;
	}
	ILogHandler*& logger::get_handler_ref()
	{
		static CerrLogHandler default_handler;
		static ILogHandler* current_handler = &default_handler;
		return current_handler;
	}

} // namespace ormcxx
