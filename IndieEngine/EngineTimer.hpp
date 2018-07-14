#ifndef ENGINE_TIMER_HPP
#define ENGINE_TIMER_HPP

class EngineTimer
{
private:
	double deltaTime;
	double secondsPerCount;

	__int64 baseTime;
	__int64 currTime;
	__int64 prevTime;
	__int64 pausedTime;
	__int64 stopTime;

	bool isStopped;
public:
	EngineTimer();

	float getTotalTime(void) const;
	float getDeltaTime(void) const;

	void start(void);
	void stop(void);
	void reset(void);
	void tick(void);
};


#endif