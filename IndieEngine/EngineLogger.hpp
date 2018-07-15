#ifndef ENGINE_LOGGER_HPP
#define ENGINE_LOGGER_HPP

#include "spdlog/spdlog.h"
#include <memory>

class EngineLogger
{
private:
	static EngineLogger* loggerInstance;
	std::shared_ptr<spdlog::logger> console;

private:
	void prepare_logdir();

public:
	EngineLogger();
	
	static EngineLogger* getInstance();
	std::shared_ptr<spdlog::logger> getConsole(void) { return console; }
};


#endif