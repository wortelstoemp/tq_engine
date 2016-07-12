/* Standard headers */
#include <stdio.h>
#include <stdlib.h>

/* Libraries */
#include <SDL.h>

/* Include own headers */
#include "types.h"
#include "platform.h"
#include "input.h"
#include "render.h"
/* #include "math.h" */

/* Clock */
Clock CreateClock(float fps)
{
	Clock result;
	result.performanceFrequency = SDL_GetPerformanceFrequency();
	result.current = SDL_GetPerformanceCounter();
	result.last = result.current;
	result.fixed = 1.0f / fps;
	result.accumulator = 0.0;
	
	return result;
}

void StartClock(Clock* clock)
{
	clock->last = SDL_GetPerformanceCounter();
}

float CalcClockDelta(Clock* clock)
{
	float delta;
	float fps;
	u64 counterDelta;
	
	clock->performanceFrequency = SDL_GetPerformanceFrequency();
	clock->current = SDL_GetPerformanceCounter();
	counterDelta = clock->current - clock->last;
	
	delta = (((1000.0f * (float) counterDelta) / (float) clock->performanceFrequency));
	
	if (delta > clock->fixed)
	{
		delta = clock->fixed;
	}
	
	fps = (float) clock->performanceFrequency / (float) counterDelta;
	/* printf("%.10f ms/f, %.10f f/s\n", delta, fps); */
	clock->last = clock->current;
	clock->accumulator += delta;
    
	return delta;
}

int IsClockAccumulating(const Clock* const clock)
{
	return (clock->accumulator >= clock->fixed);	
}

void AccumulateClock(Clock* clock)
{
	clock->accumulator -= clock->fixed;
}

float CalcClockInterpolationAlpha(const Clock* const clock)
{
	return (clock->accumulator / clock->fixed);
}

/* Window */
typedef struct Window
{
	SDL_Window* handle;
	SDL_Renderer* renderer;
	SDL_Texture* frontBuffer;
} Window;

Window CreateWindow(const char* title, int width, int height, u32 flags)
{
	if(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL could not initialize window! SDL_Error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	Window result;
	result.handle = SDL_CreateWindow(
			title,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			flags
	);
	result.renderer = SDL_CreateRenderer(result.handle, -1, SDL_RENDERER_SOFTWARE);
	result.frontBuffer = SDL_CreateTexture
	(
            result.renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
	);
	if (!result.frontBuffer)
	{
		printf("Could not create frontbuffer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	return result;	
}

void DestroyWindow(Window* window)
{
	SDL_DestroyWindow(window->handle);
	SDL_DestroyRenderer(window->renderer);	
	SDL_DestroyTexture(window->frontBuffer);
	SDL_Quit();
}

void SwapBuffers(Window* window, Renderer* renderer)
{
	SDL_UpdateTexture(window->frontBuffer, 0, renderer->backBuffer.memory, renderer->backBuffer.pitch);
	SDL_RenderCopy(window->renderer, window->frontBuffer, 0, 0);
	SDL_RenderPresent(window->renderer);
}

/* Input */
bool HandleEvents(Input* input)
{
	SDL_Event event;
    bool isRunning = true;
	InputResetMouseScroll(input);
	/* SDL_SetRelativeMouseMode(SDL_TRUE);*/
	
	for (int i = 0; i < INPUT_NUM_MOUSEBUTTONS; i++)
	{
		input->mouseButtonsDown[i] = 0;
		input->mouseButtonsUp[i] = 0;		
	}
	
	for (int i = 0; i < input->numKeys; i++)
	{
		input->keysDown[i] = 0;
		input->keysUp[i] = 0;		
	}
		
	i32 relx, rely;
	SDL_GetRelativeMouseState(&relx, &rely);
	InputSetRelativeMouseMotion(input, relx, rely);
	
	i32 x, y;
	SDL_GetMouseState(&x, &y);
	InputSetMousePosition(input, x, y);
	
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN)
		{
			int scancode = event.key.keysym.scancode;
			input->keysDown[scancode] = 1;		
		}
			
		if (event.type == SDL_KEYUP)
		{
			int scancode = event.key.keysym.scancode;
			input->keysUp[scancode] = 1;	
		}
		
		switch(event.type)
    	{
			case SDL_QUIT:
			{
				isRunning = false;
			} break;
		
			case SDL_MOUSEBUTTONDOWN:
			{
				int button = event.button.button;
				input->mouseButtons[button] = 1;
				input->mouseButtonsDown[button] = 1;
				
			} break;
			
			case SDL_MOUSEBUTTONUP:
			{
				int button = event.button.button;
				input->mouseButtons[button] = 0;
				input->mouseButtonsUp[button] = 1;
				
			} break;
		
			case SDL_MOUSEWHEEL:
			{
				SDL_MouseWheelEvent* mwEvent = (SDL_MouseWheelEvent*) &event;
				InputAddMouseScroll(input, mwEvent->x, mwEvent->y);
			} break;
		
			case SDL_WINDOWEVENT:
			{
				switch(event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
					{
						/* TODO */
					} break;
				}
			} break;
		}
	}
	
	return isRunning;
}


int main(int argc, char* argv[])
{
	/* Initialize Window */
	int width = 800;
	int height = 600;
	Window window = CreateWindow
	(
			"TQ Engine", 
			width, 
			height,
			SDL_WINDOW_SHOWN |
            /*SDL_WINDOW_RESIZABLE |*/ 
            SDL_RENDERER_PRESENTVSYNC |
            SDL_WINDOW_INPUT_FOCUS
	);
	
	Renderer renderer = CreateRenderer(width, height);
	
	int numKeys;
	u8* keyboardState = (u8*) SDL_GetKeyboardState(&numKeys);
	Input input = CreateInput(keyboardState, numKeys);
	
	/* Main loop */
	bool isRunning = true;
	float dt = 0.0f;
	Clock clock = CreateClock(60);
	StartClock(&clock);
	
	while (isRunning)
	{
		dt = CalcClockDelta(&clock);
		isRunning = HandleEvents(&input);
		
		if (input.keys[SDL_SCANCODE_W])
		{
			printf("Pressing W\n");
		}
		
		if (input.keysUp[SDL_SCANCODE_W])
		{
			printf("Released W\n");
		}
		
		while (IsClockAccumulating(&clock))
		{
			/* Fixed stuff */
			AccumulateClock(&clock);
		}
		
		Color clearColor;
		clearColor.r = 0;
		clearColor.g = 0;
		clearColor.b = 0;
		clearColor.a = 255;
		
		Color color;
		color.r = 255;
		color.g = 255;
		color.b = 255;
		color.a = 255;
		
		Color color2;
		color2.r = 0;
		color2.g = 255;
		color2.b = 0;
		color2.a = 255;
		
		
		ClearBackBuffer(&renderer, &clearColor);
		DrawPixel(&renderer, 80, 40, &color2);
		DrawTriangle(&renderer, 80, 100, 150, 200, 100, 300, &color);
		DrawTriangle(&renderer, 300, 300, 150, 400, 450, 400, &color2);
		
		SwapBuffers(&window, &renderer);
	}
	
	/* Shutdown */
	DestroyInput(&input);
	DestroyRenderer(&renderer);
	DestroyWindow(&window);

	return 0;
}
