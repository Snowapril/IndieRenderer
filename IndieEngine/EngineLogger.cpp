#include "EngineLogger.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"

EngineLogger* EngineLogger::loggerInstance = nullptr;

EngineLogger::EngineLogger()
{
	console = spdlog::stdout_color_mt("console");

#ifdef _DEBUG
	console->set_level(spdlog::level::debug);
#else
	console->set_level(spdlog::level::info);
#endif
}

void EngineLogger::prepare_logdir(void)
{
	spdlog::drop_all();
#ifdef _WIN32
	system("if not exist logs mkdir logs");
	system("del /F /Q logs\\*");
#else
	auto rv = system("mkdir -p logs");
	rv = system("rm -f logs/*");
	(void)rv;
#endif
}

EngineLogger* EngineLogger::getInstance()
{
	if (!loggerInstance) {
		loggerInstance = new EngineLogger;
	}

	return loggerInstance;
}