#pragma once

/*****************************************************************************/
/* Clock */

typedef struct Clock
{
	u64 performanceFrequency;
	u64 current;
	u64 last;
	float fixed;
	float accumulator;	
} Clock;

Clock CreateClock(float fps);
void StartClock(Clock* clock);
float CalcClockDelta(Clock* clock);
int	IsClockAccumulating(const Clock* const clock);
void AccumulateClock(Clock* clock);
float CalcClockInterpolationAlpha(const Clock* const clock);

/*****************************************************************************/
/* Window */

struct Window;
struct Renderer;

struct Window CreateWindow(const char* title, int width, int height, u32 flags);
void DestroyWindow(struct Window* window);
void SwapBuffers(struct Window* window, struct Renderer* renderer);

/*****************************************************************************/
/* Input */

struct Input;
bool HandleEvents(struct Input* input);