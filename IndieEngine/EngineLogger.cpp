#include "EngineLogger.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <vector>
#include <memory>


std::shared_ptr<spdlog::logger> EngineLogger::console = nullptr;

std::shared_ptr<spdlog::logger> EngineLogger::getConsole(void) 
{
	if (console == nullptr)
	{
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

#ifdef _DEBUG
		sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("./logs/setuplog.txt"));
#endif

		console = std::make_shared<spdlog::logger>("setup", sinks.begin(), sinks.end());

#ifdef _DEBUG
		console->set_level(spdlog::level::trace);
#else
		console->set_level(spdlog::level::info);
#endif
	}

	return console; 
}