#include "EngineLogger.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

std::shared_ptr<spdlog::logger> EngineLogger::console = nullptr;

std::shared_ptr<spdlog::logger> EngineLogger::getConsole(void) 
{
	if (console == nullptr)
	{
		console = spdlog::stdout_color_mt("console");
#ifdef _DEBUG
		console->set_level(spdlog::level::debug);
#else
		console->set_level(spdlog::level::info);
#endif
	}

	return console; 
}