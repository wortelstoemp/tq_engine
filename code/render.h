#pragma once

typedef struct BackBuffer
{
	u8*				memory;
	int 			width;
	int 			height;
	int				pitch;
	int				bytesPerPixel;
} BackBuffer;

typedef struct Color
{
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} Color;

typedef struct Renderer
{
	BackBuffer		backBuffer;
	int*			scanBuffer;
} Renderer;

Renderer CreateRenderer(int width, int height)
{
	const int bytesPerPixel = 4;
	const int pitch = width * bytesPerPixel;
	const int memorySize = pitch * height;
	
	Renderer result;
	result.backBuffer.bytesPerPixel = bytesPerPixel;
	result.backBuffer.pitch = pitch;
	result.backBuffer.memory = (u8*) malloc(sizeof(u8) * memorySize);
	
	for (int i = 0; i < memorySize; i++)
	{
		result.backBuffer.memory[i] = 0;
	}
	
	result.backBuffer.width = width;
	result.backBuffer.height = height;
	
	const int scanBufferSize = 2 * height;
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
	        /* Format: BGRA */
	        const size_t index = i * columns + j;
	        backBuffer->memory[index] = color->b;
	        backBuffer->memory[index + 1] = color->g;
	        backBuffer->memory[index + 2] = color->r;
	        backBuffer->memory[index + 3] = color->a;  
	    }
	}
}

/* TODO: Resize? */

void DrawPixel(Renderer* renderer, int x, int y, const Color* const color)
{
	BackBuffer* backBuffer = &(renderer->backBuffer);
	const int width = backBuffer->width;
	const int height = backBuffer->height;
	
	/* TODO: remove check if clipping for lines and polygons works */
	if (x > 0 && y > 0 && x < width && y < height)
	{
		const size_t index = (x + y * width) * backBuffer->bytesPerPixel;
    	backBuffer->memory[index] = color->b;
    	backBuffer->memory[index + 1] = color->g;
    	backBuffer->memory[index + 2] = color->r;
    	backBuffer->memory[index + 3] = color->a; 
	}
}

/*	Currently drawing with Digital Differential Analyzer (DDA)
 *	TODO: Change to Bresenham's line drawing algorithm
 */
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

/* TODO: make this working for every slope */
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

/* TODO: Optimize for parallellisation */
void DrawTriangle(Renderer* renderer, int v1x, int v1y,
	int v2x, int v2y, 
	int v3x, int v3y,
	const Color* const color)
{	
	/* 1) Preprocessing: Convert triangle to scan-buffer-friendly triangle */
	/* 1a) Perspective divide */
	
	/* 1b) Viewport transform */
	
	/* 1c) Bubble sort vertices by y-coordinate in increasing order */
	if (v1y > v2y)
	{
		/* swap v1 v2 */
		int temp = v1x;
		v1x = v2x;
		v2x = temp;
		temp = v1y;
		v1y = v2y;
		v2y = temp;
	}	
	if (v2y > v3y)
	{
		/* swap v2 v3 */
		int temp = v2x;
		v2x = v3x;
		v3x = temp;
		temp = v2y;
		v2y = v3y;
		v3y = temp;
	}
	if (v1y > v2y)
	{
		/* swap v1 v2 */
		int temp = v1x;
		v1x = v2x;
		v2x = temp;
		temp = v1y;
		v1y = v2y;
		v2y = temp;
	}
	
	/* 1d) Determine handedness by area of parallellogram
	 *		Area: 2D cross product */
	int area = (v3x - v1x) * (v2y - v1y) - (v3y - v1y) * (v2x - v1x);
	int handedness = (area >= 0) 
		? 1		/* v1v2 is to the right of v1v3 */
		: 0;	/* v1v2 is to the left of v1v3 */
	
	/* 2) Fill scan buffer
	 *		First define min side for scanbuffer
	 *		Next define the two max sides for scanbuffer */
	ScanConvertLine(renderer, v1x, v1y, v3x, v3y, handedness);
	ScanConvertLine(renderer, v1x, v1y, v2x, v2y, 1 - handedness);
	ScanConvertLine(renderer, v2x, v2y, v3x, v3y, 1 - handedness);
	
	/* 3)	Read scan buffer and draw to back buffer */
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