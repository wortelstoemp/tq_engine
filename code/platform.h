#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef unsigned char uchar;
typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef unsigned int uint;
typedef unsigned long ulong;

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

struct Window SDL2CreateWindow(const char* title, int width, int height, u32 flags);
void SDL2DestroyWindow(struct Window* window);
void SDL2SwapBuffers(struct Window* window, struct Renderer* renderer);

/*****************************************************************************/
/* Input */

struct Input;
static bool HandleEvents(struct Input* input);