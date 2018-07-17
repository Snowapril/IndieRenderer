#ifndef ENGINE_LOGGER_HPP
#define ENGINE_LOGGER_HPP

#include "spdlog/spdlog.h"
#include <memory>
#include <unordered_map>

class EngineLogger
{
private:
	static std::shared_ptr<spdlog::logger> console;

public:
	static std::shared_ptr<spdlog::logger> getConsole(void);

};


#endif