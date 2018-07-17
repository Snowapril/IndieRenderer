#ifndef ENGINE_PROFILER_HPP
#define ENGINE_PROFILER_HPP

#include <unordered_map>

typedef struct _cellInfo {
	int callCount;
	double totalTime;
} Cell;

class EngineProfiler
{
private:
	static std::unordered_map<std::string, Cell> profileInfo;
	static std::string dump(void);
	static double secondsPerCount;
	
	std::string currentLabel;
	__int64 startTime;
public:
	EngineProfiler(const char* filename, const char* functionName);
	~EngineProfiler();
};

#define Profile() EngineProfiler(__FILE__, __FUNCTION__);

#endif