// Standard headers
#include <stdio.h>
#include <stdlib.h>

// Libraries
#include <SDL.h>

// Include own headers
#include "types.h"

// Clock
typedef struct
{
	uint64  performanceFrequency;
    uint64  current;
    uint64  last;
    float   fixed;
    float   accumulator;	
} Clock;

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

int IsClockAccumulating(const Clock* const clock)
{
	return (clock->accumulator >= clock->fixed);	
}

void AccumulateClock(Clock* clock)
{
	clock->accumulator -= clock->fixed;
}

float CalcClockDelta(Clock* clock)
{
	float delta;
	float fps;
	uint64 counterDelta;
	
	clock->performanceFrequency = SDL_GetPerformanceFrequency();
	clock->current = SDL_GetPerformanceCounter();
	counterDelta = clock->current - clock->last;
	
	delta = (((1000.0f * (float) counterDelta) / (float) clock->performanceFrequency));
	
	if (delta > clock->fixed)
	{
		delta = clock->fixed;
	}
	
	fps = (float) clock->performanceFrequency / (float) counterDelta;
	//printf("%.10f ms/f, %.10f f/s\n", delta, fps);
	clock->last = clock->current;
	clock->accumulator += delta;
    
	return delta;
}

void StartClock(Clock* clock)
{
	clock->last = SDL_GetPerformanceCounter();
}

float CalcClockInterpolationAlpha(const Clock* const clock)
{
	return (clock->accumulator / clock->fixed);
}

// Color
typedef struct
{
	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;
} Color;

// Window
typedef struct
{
	SDL_Window* content;
} Window;

typedef struct
{
	int width;
	int height;
} WindowDimension;

Window CreateWindow(const char* title, int width, int height, uint32 flags)
{
	if(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
		printf("SDL could not initialize window! SDL_Error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	Window result;
	result.content = SDL_CreateWindow(
			title,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			flags
	);
	
	return result;	
}

void DestroyWindow(Window* window)
{
	SDL_DestroyWindow(window->content);
	SDL_Quit();
}

// Input 
bool HandleEvent(SDL_Event* event)
{
    bool isRunning = true;
    switch(event->type)
    {
        case SDL_QUIT:
        {
            isRunning = false;
        } break;
        
        case SDL_WINDOWEVENT:
        {
            switch(event->window.event)
            {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    // TODO
                } break;
            }
        } break;
    }
    
	return isRunning;
}

// Software Renderer
typedef struct
{
	uint8*			memory;
	int 			width;
	int 			height;
	int				pitch;
	int				bytesPerPixel;
} BackBuffer;

typedef struct
{
	SDL_Renderer* 	content;
	SDL_Texture*	frontBuffer;
	BackBuffer		backBuffer;
	int*			scanBuffer;
} Renderer;

Renderer CreateRenderer(Window* window)
{
	WindowDimension dimension;
	SDL_GetWindowSize(window->content, &dimension.width, &dimension.height);
	
	Renderer result;
	result.content = SDL_CreateRenderer(window->content, -1, SDL_RENDERER_SOFTWARE);
	result.frontBuffer = SDL_CreateTexture(
            result.content,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            dimension.width,
            dimension.height);
	if (!result.frontBuffer)
	{
		printf("Could not create frontbuffer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	const int bytesPerPixel = 4;
	const int pitch = dimension.width * bytesPerPixel;
	const int memorySize = pitch * dimension.height;
	
	result.backBuffer.bytesPerPixel = bytesPerPixel;
	result.backBuffer.pitch = pitch;
	result.backBuffer.memory = (uint8*) malloc(sizeof(uint8) * memorySize);
	
	for (int i = 0; i < memorySize; i++)
	{
		result.backBuffer.memory[i] = 0;
	}
	
	result.backBuffer.width = dimension.width;
	result.backBuffer.height = dimension.height;
	
	const int scanBufferSize = 2 * dimension.height;
	result.scanBuffer = (int*) malloc(sizeof(int) * scanBufferSize);
	for (int i = 0; i < scanBufferSize; i++)
	{
		result.scanBuffer[i] = 0;
	}
	
	return result;
}

void DestroyRenderer(Renderer* renderer)
{
	free(renderer->backBuffer.memory);
	free(renderer->scanBuffer);
	SDL_DestroyTexture(renderer->frontBuffer);
}

void ClearBackBuffer(Renderer* renderer, Color* color)
{
	BackBuffer* backBuffer = &renderer->backBuffer;
	const size_t columns = backBuffer->width * backBuffer->bytesPerPixel;
	const size_t rows = backBuffer->height;
        
	for (size_t i = 0; i < rows; i++)
	{
	    for (size_t j = 0; j < columns; j += backBuffer->bytesPerPixel)
		{
	        // Format: BGRA
	        const size_t index = i * columns + j;
	        backBuffer->memory[index] = color->b;
	        backBuffer->memory[index + 1] = color->g;
	        backBuffer->memory[index + 2] = color->r;
	        backBuffer->memory[index + 3] = color->a;  
	    }
	}
}

// TODO: Resize?

void SwapBuffers(Renderer* renderer)
{
	SDL_UpdateTexture(renderer->frontBuffer, 0, renderer->backBuffer.memory, renderer->backBuffer.pitch);
	SDL_RenderCopy(renderer->content, renderer->frontBuffer, 0, 0);
	SDL_RenderPresent(renderer->content);
}

void DrawPixel(Renderer* renderer, int x, int y, const Color* const color)
{
	BackBuffer* backBuffer = &(renderer->backBuffer);
	const int width = backBuffer->width;
    const int height = backBuffer->height;
	
	// x += width / 2;
	// y = height / 2 - y;	
	
	// TODO: remove check if clipping for lines and polygons works
	if (x > 0 && y > 0 && x < width && y < height)
	{
		const size_t index = (x + y * width) * backBuffer->bytesPerPixel;
    	backBuffer->memory[index] = color->b;
    	backBuffer->memory[index + 1] = color->g;
    	backBuffer->memory[index + 2] = color->r;
    	backBuffer->memory[index + 3] = color->a; 
	}
}

// Currently drawing with Digital Differential Analyzer (DDA)
// TODO: Change to Bresenham's line drawing algorithm
void DrawLine(Renderer* renderer, int x0, int y0, int x1, int y1, const Color* const color)
{
	float m = (y1 - y0) / (float) (x1 - x0);
	
	if (x0 <= x1)
	{
		if (m > 1.0f)
		{
		    m = 1.0f / m;
		    float x = (float) x0;
		    for (int y = y0; y <= y1; y++, x += m)
		        DrawPixel(renderer, (int) floor(x + 0.5f), y, color);
		}
		else if (m < -1.0f)
		{
		    m = 1.0f / m;
		    float x = (float) x0;
		    for (int y = y0; y >= y1; y--, x -= m)
		        DrawPixel(renderer, (int) floor(x - 0.5f), y, color);
		}
		else
		{
		    float y = (float) y0;
		    for (int x = x0; x <= x1; x++, y += m)
		        DrawPixel(renderer, x, (int) floor(y + 0.5f), color);
		}
	}
	else
	{
		if (m > 1.0f)
		{
		    m = 1.0f / m;
		    float x = (float) x0;
		    for (int y = y0; y >= y1; y--, x -= m)
		        DrawPixel(renderer, (int) floor(x - 0.5f), y, color);
		}
		else if (m < -1.0f)
		{
		    m = 1.0f / m;
		    float x = (float) x0;
		    for (int y = y0; y <= y1; y++, x += m)
		        DrawPixel(renderer, (int) floor(x + 0.5f), y, color);
		}
		else
		{
		    float y = (float) y1;
		    for (int x = x1; x <= x0; x++, y += m)
		        DrawPixel(renderer, x, (int) floor(y + 0.5), color);    
		}
	}
}

void DrawLineBresenham(Renderer* renderer, int x0, int y0, int x1, int y1, const Color* const color)
{
	const int dy = y1 - y0;
	const int dx = x1 - x0;
	const int incrE = 2 * dy;
	const int incrNE = 2 * (dy - dx);
	int d = incrE - dx;
	int y = y0;
	int x = x0;	
	
	DrawPixel(renderer, x, y, color);
	
	while (x < x1)
	{
		if (d <= 0)
		{
			d += incrE;
		}
		else
		{
			d += incrNE;
			y++;
		}
		x++;
		DrawPixel(renderer, x, y, color);
	}
}

void ScanConvertLine(Renderer* renderer, int x0, int y0, int x1, int y1, int isMaxSide)
{
	const int dx = x1 - x0;
	const int dy = y1 - y0;
	
	if (dy <= 0)
		return;
	
	const float m = (float) dx / dy;
	float x = (float) x0;
	
	for (int y = y0; y < y1; y++)
	{
		renderer->scanBuffer[y * 2 + isMaxSide] = (int) x;
		x += m;
	}
}

void DrawTriangle(Renderer* renderer, int v1x, int v1y,
	int v2x, int v2y, 
	int v3x, int v3y,
	const Color* const color)
{
	// 1) Preprocessing: Convert triangle to scan-buffer-friendly triangle
	// 1a) Bubble sort vertices by y-coordinate in increasing order
	if (v1y > v2y)
	{
		// swap v1 v2
		int temp = v1x;
		v1x = v2x;
		v2x = temp;
		temp = v1y;
		v1y = v2y;
		v2y = temp;
	}	
	if (v2y > v3y)
	{
		// swap v2 v3
		int temp = v2x;
		v2x = v3x;
		v3x = temp;
		temp = v2y;
		v2y = v3y;
		v3y = temp;
	}
	if (v1y > v2y)
	{
		// swap v1 v2
		int temp = v1x;
		v1x = v2x;
		v2x = temp;
		temp = v1y;
		v1y = v2y;
		v2y = temp;
	}
	
	// 1b) Determine handedness by area of parallellogram
	// Area: 2D cross product
	int area = (v3x - v1x) * (v2y - v1y) - (v3y - v1y) * (v2x - v1x);
	int handedness = (area >= 0) 
		? 1		// v1v2 is to the right of v1v3
		: 0;	// v1v2 is to the left of v1v3
	
	// 2) Fill scan buffer
	// First define min side for scanbuffer
	// Next define the two max sides for scanbuffer
	ScanConvertLine(renderer, v1x, v1y, v3x, v3y, handedness);
	ScanConvertLine(renderer, v1x, v1y, v2x, v2y, 1 - handedness);
	ScanConvertLine(renderer, v2x, v2y, v3x, v3y, 1 - handedness);
	
	// 3) Read scan buffer and draw to back buffer
	int* scanBuffer = renderer->scanBuffer;
	int yMin = v1y; 
	int yMax = v3y;	
	
	for (int j = yMin; j < yMax; j++)
	{			
		const size_t minIndex = j * 2;
		const size_t maxIndex = minIndex + 1;
		const int xMin = scanBuffer[minIndex];
		const int xMax = scanBuffer[maxIndex];
				
		for (int i = xMin; i < xMax; i++)
		{
			DrawPixel(renderer, i, j, color);
		}	
	}
}


int main(int argc, char* argv[])
{
	// Initialize Window
	Window window = CreateWindow
	(
			"TQ Engine", 
			800, 
			600,
			SDL_WINDOW_SHOWN |
            /*SDL_WINDOW_RESIZABLE |*/ 
            SDL_RENDERER_PRESENTVSYNC |
            SDL_WINDOW_INPUT_FOCUS
	);
	
	Renderer renderer = CreateRenderer(&window);
	
	// Main loop
	bool isRunning = true;
	float dt = 0.0f;
	Clock clock = CreateClock(60);
	StartClock(&clock);
	
	while (isRunning)
	{
		dt = CalcClockDelta(&clock);
		SDL_Event event;
        
		while(SDL_PollEvent(&event))
		{
            isRunning = HandleEvent(&event);            
        }
		
		while (IsClockAccumulating(&clock)) {
            // Fixed stuff
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
		
		SwapBuffers(&renderer);
	}
	
	// Shutdown
	DestroyRenderer(&renderer);
	DestroyWindow(&window);

	return 0;
}
