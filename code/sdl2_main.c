#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sdl/SDL.h>
#include "sdl_vulkan.h"

#include "platform.h"
#include "input.h"
#include "render.h"
/* #include "math.h" */

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
	
	if (delta > clock->fixed) {
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

typedef struct Window
{
	SDL_Window* handle;
	SDL_Renderer* renderer;
	SDL_Texture* frontBuffer;
} Window;

Window SDL2CreateWindow(const char* title, int width, int height, u32 flags)
{
	if(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
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
	result.frontBuffer = SDL_CreateTexture(
            result.renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
	);
	if (!result.frontBuffer) {
		printf("Could not create frontbuffer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	return result;	
}

void SDL2DestroyWindow(Window* window)
{
	SDL_DestroyWindow(window->handle);
	SDL_DestroyRenderer(window->renderer);	
	SDL_DestroyTexture(window->frontBuffer);
	SDL_Quit();
}

void SDL2SwapBuffers(Window* window, Renderer* renderer)
{
	SDL_UpdateTexture(window->frontBuffer, 0, renderer->backBuffer.memory, renderer->backBuffer.pitch);
	SDL_RenderCopy(window->renderer, window->frontBuffer, 0, 0);
	SDL_RenderPresent(window->renderer);
}

enum Scancode
{
	INPUT_SCANCODE_UNKNOWN = SDL_SCANCODE_UNKNOWN,
	INPUT_SCANCODE_A = SDL_SCANCODE_A,
	INPUT_SCANCODE_B = SDL_SCANCODE_B,
	INPUT_SCANCODE_C = SDL_SCANCODE_C,
	INPUT_SCANCODE_D = SDL_SCANCODE_D,
	INPUT_SCANCODE_E = SDL_SCANCODE_E,
	INPUT_SCANCODE_F = SDL_SCANCODE_F,
	INPUT_SCANCODE_G = SDL_SCANCODE_G,
	INPUT_SCANCODE_H = SDL_SCANCODE_H,
	INPUT_SCANCODE_I = SDL_SCANCODE_I,
	INPUT_SCANCODE_J = SDL_SCANCODE_J,
	INPUT_SCANCODE_K = SDL_SCANCODE_K,
	INPUT_SCANCODE_L = SDL_SCANCODE_L,
	INPUT_SCANCODE_M = SDL_SCANCODE_M,
	INPUT_SCANCODE_N = SDL_SCANCODE_N,
	INPUT_SCANCODE_O = SDL_SCANCODE_O,
	INPUT_SCANCODE_P = SDL_SCANCODE_P,
	INPUT_SCANCODE_Q = SDL_SCANCODE_Q,
	INPUT_SCANCODE_R = SDL_SCANCODE_R,
	INPUT_SCANCODE_S = SDL_SCANCODE_S,
	INPUT_SCANCODE_T = SDL_SCANCODE_T,
	INPUT_SCANCODE_U = SDL_SCANCODE_U,
	INPUT_SCANCODE_V = SDL_SCANCODE_V,
	INPUT_SCANCODE_W = SDL_SCANCODE_W,
	INPUT_SCANCODE_X = SDL_SCANCODE_X,
	INPUT_SCANCODE_Y = SDL_SCANCODE_Y,
	INPUT_SCANCODE_Z = SDL_SCANCODE_Z,
	INPUT_SCANCODE_1 = SDL_SCANCODE_1,
	INPUT_SCANCODE_2 = SDL_SCANCODE_2,
	INPUT_SCANCODE_3 = SDL_SCANCODE_3,
	INPUT_SCANCODE_4 = SDL_SCANCODE_4,
	INPUT_SCANCODE_5 = SDL_SCANCODE_5,
	INPUT_SCANCODE_6 = SDL_SCANCODE_6,
	INPUT_SCANCODE_7 = SDL_SCANCODE_7,
	INPUT_SCANCODE_8 = SDL_SCANCODE_8,
	INPUT_SCANCODE_9 = SDL_SCANCODE_9,
	INPUT_SCANCODE_0 = SDL_SCANCODE_0,
	INPUT_SCANCODE_RETURN = SDL_SCANCODE_RETURN,
	INPUT_SCANCODE_ESCAPE = SDL_SCANCODE_ESCAPE,
	INPUT_SCANCODE_BACKSPACE = SDL_SCANCODE_BACKSPACE,
	INPUT_SCANCODE_TAB = SDL_SCANCODE_TAB,
	INPUT_SCANCODE_SPACE = SDL_SCANCODE_SPACE,
	INPUT_SCANCODE_MINUS = SDL_SCANCODE_MINUS,
	INPUT_SCANCODE_EQUALS = SDL_SCANCODE_EQUALS,
	INPUT_SCANCODE_LEFTBRACKET = SDL_SCANCODE_LEFTBRACKET,
	INPUT_SCANCODE_RIGHTBRACKET = SDL_SCANCODE_RIGHTBRACKET,
	INPUT_SCANCODE_BACKSLASH = SDL_SCANCODE_BACKSLASH,
	INPUT_SCANCODE_NONUSHASH = SDL_SCANCODE_NONUSHASH,
	INPUT_SCANCODE_SEMICOLON = SDL_SCANCODE_SEMICOLON,
	INPUT_SCANCODE_APOSTROPHE = SDL_SCANCODE_APOSTROPHE,
	INPUT_SCANCODE_GRAVE = SDL_SCANCODE_GRAVE,
	INPUT_SCANCODE_COMMA = SDL_SCANCODE_COMMA,
	INPUT_SCANCODE_PERIOD = SDL_SCANCODE_PERIOD,
	INPUT_SCANCODE_SLASH = SDL_SCANCODE_SLASH,
	INPUT_SCANCODE_CAPSLOCK = SDL_SCANCODE_CAPSLOCK,
	INPUT_SCANCODE_F1 = SDL_SCANCODE_F1,
	INPUT_SCANCODE_F2 = SDL_SCANCODE_F2,
	INPUT_SCANCODE_F3 = SDL_SCANCODE_F3,
	INPUT_SCANCODE_F4 = SDL_SCANCODE_F4,
	INPUT_SCANCODE_F5 = SDL_SCANCODE_F5,
	INPUT_SCANCODE_F6 = SDL_SCANCODE_F6,
	INPUT_SCANCODE_F7 = SDL_SCANCODE_F7,
	INPUT_SCANCODE_F8 = SDL_SCANCODE_F8,
	INPUT_SCANCODE_F9 = SDL_SCANCODE_F9,
	INPUT_SCANCODE_F10 = SDL_SCANCODE_F10,
	INPUT_SCANCODE_F11 = SDL_SCANCODE_F11,
	INPUT_SCANCODE_F12 = SDL_SCANCODE_F12,
	INPUT_SCANCODE_PRINTSCREEN = SDL_SCANCODE_PRINTSCREEN,
	INPUT_SCANCODE_SCROLLLOCK = SDL_SCANCODE_SCROLLLOCK,
	INPUT_SCANCODE_PAUSE = SDL_SCANCODE_PAUSE,
	INPUT_SCANCODE_INSERT = SDL_SCANCODE_INSERT,
	INPUT_SCANCODE_HOME = SDL_SCANCODE_HOME,
	INPUT_SCANCODE_PAGEUP = SDL_SCANCODE_PAGEUP,
	INPUT_SCANCODE_DELETE = SDL_SCANCODE_DELETE,
	INPUT_SCANCODE_END = SDL_SCANCODE_END,
	INPUT_SCANCODE_PAGEDOWN = SDL_SCANCODE_PAGEDOWN,
	INPUT_SCANCODE_RIGHT = SDL_SCANCODE_RIGHT,
	INPUT_SCANCODE_LEFT = SDL_SCANCODE_LEFT,
	INPUT_SCANCODE_DOWN = SDL_SCANCODE_DOWN,
	INPUT_SCANCODE_UP = SDL_SCANCODE_UP,
	INPUT_SCANCODE_NUMLOCKCLEAR = SDL_SCANCODE_NUMLOCKCLEAR,
	INPUT_SCANCODE_KP_DIVIDE = SDL_SCANCODE_KP_DIVIDE,
	INPUT_SCANCODE_KP_MULTIPLY = SDL_SCANCODE_KP_MULTIPLY,
	INPUT_SCANCODE_KP_MINUS = SDL_SCANCODE_KP_MINUS,
	INPUT_SCANCODE_KP_PLUS = SDL_SCANCODE_KP_PLUS,
	INPUT_SCANCODE_KP_ENTER = SDL_SCANCODE_KP_ENTER,
	INPUT_SCANCODE_KP_1 = SDL_SCANCODE_KP_1,
	INPUT_SCANCODE_KP_2 = SDL_SCANCODE_KP_2,
	INPUT_SCANCODE_KP_3 = SDL_SCANCODE_KP_3,
	INPUT_SCANCODE_KP_4 = SDL_SCANCODE_KP_4,
	INPUT_SCANCODE_KP_5 = SDL_SCANCODE_KP_5,
	INPUT_SCANCODE_KP_6 = SDL_SCANCODE_KP_6,
	INPUT_SCANCODE_KP_7 = SDL_SCANCODE_KP_7,
	INPUT_SCANCODE_KP_8 = SDL_SCANCODE_KP_8,
	INPUT_SCANCODE_KP_9 = SDL_SCANCODE_KP_9,
	INPUT_SCANCODE_KP_0 = SDL_SCANCODE_KP_0,
	INPUT_SCANCODE_KP_PERIOD = SDL_SCANCODE_KP_PERIOD,
	INPUT_SCANCODE_NONUSBACKSLASH = SDL_SCANCODE_NONUSBACKSLASH,
	INPUT_SCANCODE_APPLICATION = SDL_SCANCODE_APPLICATION,
	INPUT_SCANCODE_POWER = SDL_SCANCODE_POWER,
	INPUT_SCANCODE_KP_EQUALS = SDL_SCANCODE_KP_EQUALS,
	INPUT_SCANCODE_F13 = SDL_SCANCODE_F13,
	INPUT_SCANCODE_F14 = SDL_SCANCODE_F14,
	INPUT_SCANCODE_F15 = SDL_SCANCODE_F15,
	INPUT_SCANCODE_F16 = SDL_SCANCODE_F16,
	INPUT_SCANCODE_F17 = SDL_SCANCODE_F17,
	INPUT_SCANCODE_F18 = SDL_SCANCODE_F18,
	INPUT_SCANCODE_F19 = SDL_SCANCODE_F19,
	INPUT_SCANCODE_F20 = SDL_SCANCODE_F20,
	INPUT_SCANCODE_F21 = SDL_SCANCODE_F21,
	INPUT_SCANCODE_F22 = SDL_SCANCODE_F22,
	INPUT_SCANCODE_F23 = SDL_SCANCODE_F23,
	INPUT_SCANCODE_F24 = SDL_SCANCODE_F24,
	INPUT_SCANCODE_EXECUTE = SDL_SCANCODE_EXECUTE,
	INPUT_SCANCODE_HELP = SDL_SCANCODE_HELP,
	INPUT_SCANCODE_MENU = SDL_SCANCODE_MENU,
	INPUT_SCANCODE_SELECT = SDL_SCANCODE_SELECT,
	INPUT_SCANCODE_STOP = SDL_SCANCODE_STOP,
	INPUT_SCANCODE_AGAIN = SDL_SCANCODE_AGAIN,
	INPUT_SCANCODE_UNDO = SDL_SCANCODE_UNDO,
	INPUT_SCANCODE_CUT = SDL_SCANCODE_CUT,
	INPUT_SCANCODE_COPY = SDL_SCANCODE_COPY,
	INPUT_SCANCODE_PASTE = SDL_SCANCODE_PASTE,
	INPUT_SCANCODE_FIND = SDL_SCANCODE_FIND,
	INPUT_SCANCODE_MUTE = SDL_SCANCODE_MUTE,
	INPUT_SCANCODE_VOLUMEUP = SDL_SCANCODE_VOLUMEUP,
	INPUT_SCANCODE_VOLUMEDOWN = SDL_SCANCODE_VOLUMEDOWN,
	INPUT_SCANCODE_KP_COMMA = SDL_SCANCODE_KP_COMMA,
	INPUT_SCANCODE_KP_EQUALSAS400 = SDL_SCANCODE_KP_EQUALSAS400,
	INPUT_SCANCODE_INTERNATIONAL1 = SDL_SCANCODE_INTERNATIONAL1,
	INPUT_SCANCODE_INTERNATIONAL2 = SDL_SCANCODE_INTERNATIONAL2,
	INPUT_SCANCODE_INTERNATIONAL3 = SDL_SCANCODE_INTERNATIONAL3,
	INPUT_SCANCODE_INTERNATIONAL4 = SDL_SCANCODE_INTERNATIONAL4,
	INPUT_SCANCODE_INTERNATIONAL5 = SDL_SCANCODE_INTERNATIONAL5,
	INPUT_SCANCODE_INTERNATIONAL6 = SDL_SCANCODE_INTERNATIONAL6,
	INPUT_SCANCODE_INTERNATIONAL7 = SDL_SCANCODE_INTERNATIONAL7,
	INPUT_SCANCODE_INTERNATIONAL8 = SDL_SCANCODE_INTERNATIONAL8,
	INPUT_SCANCODE_INTERNATIONAL9 = SDL_SCANCODE_INTERNATIONAL9,
	INPUT_SCANCODE_LANG1 = SDL_SCANCODE_LANG1,
	INPUT_SCANCODE_LANG2 = SDL_SCANCODE_LANG2,
	INPUT_SCANCODE_LANG3 = SDL_SCANCODE_LANG3,
	INPUT_SCANCODE_LANG4 = SDL_SCANCODE_LANG4,
	INPUT_SCANCODE_LANG5 = SDL_SCANCODE_LANG5,
	INPUT_SCANCODE_LANG6 = SDL_SCANCODE_LANG6,
	INPUT_SCANCODE_LANG7 = SDL_SCANCODE_LANG7,
	INPUT_SCANCODE_LANG8 = SDL_SCANCODE_LANG8,
	INPUT_SCANCODE_LANG9 = SDL_SCANCODE_LANG9,
	INPUT_SCANCODE_ALTERASE = SDL_SCANCODE_ALTERASE,
	INPUT_SCANCODE_SYSREQ = SDL_SCANCODE_SYSREQ,
	INPUT_SCANCODE_CANCEL = SDL_SCANCODE_CANCEL,
	INPUT_SCANCODE_CLEAR = SDL_SCANCODE_CLEAR,
	INPUT_SCANCODE_PRIOR = SDL_SCANCODE_PRIOR,
	INPUT_SCANCODE_RETURN2 = SDL_SCANCODE_RETURN2,
	INPUT_SCANCODE_SEPARATOR = SDL_SCANCODE_SEPARATOR,
	INPUT_SCANCODE_OUT = SDL_SCANCODE_OUT,
	INPUT_SCANCODE_OPER = SDL_SCANCODE_OPER,
	INPUT_SCANCODE_CLEARAGAIN = SDL_SCANCODE_CLEARAGAIN,
	INPUT_SCANCODE_CRSEL = SDL_SCANCODE_CRSEL,
	INPUT_SCANCODE_EXSEL = SDL_SCANCODE_EXSEL,
	INPUT_SCANCODE_KP_00 = SDL_SCANCODE_KP_00,
	INPUT_SCANCODE_KP_000 = SDL_SCANCODE_KP_000,
	INPUT_SCANCODE_THOUSANDSSEPARATOR = SDL_SCANCODE_THOUSANDSSEPARATOR,
	INPUT_SCANCODE_DECIMALSEPARATOR = SDL_SCANCODE_DECIMALSEPARATOR,
	INPUT_SCANCODE_CURRENCYUNIT = SDL_SCANCODE_CURRENCYUNIT,
	INPUT_SCANCODE_CURRENCYSUBUNIT = SDL_SCANCODE_CURRENCYSUBUNIT,
	INPUT_SCANCODE_KP_LEFTPAREN = SDL_SCANCODE_KP_LEFTPAREN,
	INPUT_SCANCODE_KP_RIGHTPAREN = SDL_SCANCODE_KP_RIGHTPAREN,
	INPUT_SCANCODE_KP_LEFTBRACE = SDL_SCANCODE_KP_LEFTBRACE,
	INPUT_SCANCODE_KP_RIGHTBRACE = SDL_SCANCODE_KP_RIGHTBRACE,
	INPUT_SCANCODE_KP_TAB = SDL_SCANCODE_KP_TAB,
	INPUT_SCANCODE_KP_BACKSPACE = SDL_SCANCODE_KP_BACKSPACE,
	INPUT_SCANCODE_KP_A = SDL_SCANCODE_KP_A,
	INPUT_SCANCODE_KP_B = SDL_SCANCODE_KP_B,
	INPUT_SCANCODE_KP_C = SDL_SCANCODE_KP_C,
	INPUT_SCANCODE_KP_D = SDL_SCANCODE_KP_D,
	INPUT_SCANCODE_KP_E = SDL_SCANCODE_KP_E,
	INPUT_SCANCODE_KP_F = SDL_SCANCODE_KP_F,
	INPUT_SCANCODE_KP_XOR = SDL_SCANCODE_KP_XOR,
	INPUT_SCANCODE_KP_POWER = SDL_SCANCODE_KP_POWER,
	INPUT_SCANCODE_KP_PERCENT = SDL_SCANCODE_KP_PERCENT,
	INPUT_SCANCODE_KP_LESS = SDL_SCANCODE_KP_LESS,
	INPUT_SCANCODE_KP_GREATER = SDL_SCANCODE_KP_GREATER,
	INPUT_SCANCODE_KP_AMPERSAND = SDL_SCANCODE_KP_AMPERSAND,
	INPUT_SCANCODE_KP_DBLAMPERSAND = SDL_SCANCODE_KP_DBLAMPERSAND,
	INPUT_SCANCODE_KP_VERTICALBAR = SDL_SCANCODE_KP_VERTICALBAR,
	INPUT_SCANCODE_KP_DBLVERTICALBAR = SDL_SCANCODE_KP_DBLVERTICALBAR,
	INPUT_SCANCODE_KP_COLON = SDL_SCANCODE_KP_COLON,
	INPUT_SCANCODE_KP_HASH = SDL_SCANCODE_KP_HASH,
	INPUT_SCANCODE_KP_SPACE = SDL_SCANCODE_KP_SPACE,
	INPUT_SCANCODE_KP_AT = SDL_SCANCODE_KP_AT,
	INPUT_SCANCODE_KP_EXCLAM = SDL_SCANCODE_KP_EXCLAM,
	INPUT_SCANCODE_KP_MEMSTORE = SDL_SCANCODE_KP_MEMSTORE,
	INPUT_SCANCODE_KP_MEMRECALL = SDL_SCANCODE_KP_MEMRECALL,
	INPUT_SCANCODE_KP_MEMCLEAR = SDL_SCANCODE_KP_MEMCLEAR,
	INPUT_SCANCODE_KP_MEMADD = SDL_SCANCODE_KP_MEMADD,
	INPUT_SCANCODE_KP_MEMSUBTRACT = SDL_SCANCODE_KP_MEMSUBTRACT,
	INPUT_SCANCODE_KP_MEMMULTIPLY = SDL_SCANCODE_KP_MEMMULTIPLY,
	INPUT_SCANCODE_KP_MEMDIVIDE = SDL_SCANCODE_KP_MEMDIVIDE,
	INPUT_SCANCODE_KP_PLUSMINUS = SDL_SCANCODE_KP_PLUSMINUS,
	INPUT_SCANCODE_KP_CLEAR = SDL_SCANCODE_KP_CLEAR,
	INPUT_SCANCODE_KP_CLEARENTRY = SDL_SCANCODE_KP_CLEARENTRY,
	INPUT_SCANCODE_KP_BINARY = SDL_SCANCODE_KP_BINARY,
	INPUT_SCANCODE_KP_OCTAL = SDL_SCANCODE_KP_OCTAL,
	INPUT_SCANCODE_KP_DECIMAL = SDL_SCANCODE_KP_DECIMAL,
	INPUT_SCANCODE_KP_HEXADECIMAL = SDL_SCANCODE_KP_HEXADECIMAL,
	INPUT_SCANCODE_LCTRL = SDL_SCANCODE_LCTRL,
	INPUT_SCANCODE_LSHIFT = SDL_SCANCODE_LSHIFT,
	INPUT_SCANCODE_LALT = SDL_SCANCODE_LALT,
	INPUT_SCANCODE_LGUI = SDL_SCANCODE_LGUI,
	INPUT_SCANCODE_RCTRL = SDL_SCANCODE_RCTRL,
	INPUT_SCANCODE_RSHIFT = SDL_SCANCODE_RSHIFT,
	INPUT_SCANCODE_RALT = SDL_SCANCODE_RALT,
	INPUT_SCANCODE_RGUI = SDL_SCANCODE_RGUI,
	INPUT_SCANCODE_MODE = SDL_SCANCODE_MODE,
	INPUT_SCANCODE_AUDIONEXT = SDL_SCANCODE_AUDIONEXT,
	INPUT_SCANCODE_AUDIOPREV = SDL_SCANCODE_AUDIOPREV,
	INPUT_SCANCODE_AUDIOSTOP = SDL_SCANCODE_AUDIOSTOP,
	INPUT_SCANCODE_AUDIOPLAY = SDL_SCANCODE_AUDIOPLAY,
	INPUT_SCANCODE_AUDIOMUTE = SDL_SCANCODE_AUDIOMUTE,
	INPUT_SCANCODE_MEDIASELECT = SDL_SCANCODE_MEDIASELECT,
	INPUT_SCANCODE_WWW = SDL_SCANCODE_WWW,
	INPUT_SCANCODE_MAIL = SDL_SCANCODE_MAIL,
	INPUT_SCANCODE_CALCULATOR = SDL_SCANCODE_CALCULATOR,
	INPUT_SCANCODE_COMPUTER = SDL_SCANCODE_COMPUTER,
	INPUT_SCANCODE_AC_SEARCH = SDL_SCANCODE_AC_SEARCH,
	INPUT_SCANCODE_AC_HOME = SDL_SCANCODE_AC_HOME,
	INPUT_SCANCODE_AC_BACK = SDL_SCANCODE_AC_BACK,
	INPUT_SCANCODE_AC_FORWARD = SDL_SCANCODE_AC_FORWARD,
	INPUT_SCANCODE_AC_STOP = SDL_SCANCODE_AC_STOP,
	INPUT_SCANCODE_AC_REFRESH = SDL_SCANCODE_AC_REFRESH,
	INPUT_SCANCODE_AC_BOOKMARKS = SDL_SCANCODE_AC_BOOKMARKS,
	INPUT_SCANCODE_BRIGHTNESSDOWN = SDL_SCANCODE_BRIGHTNESSDOWN,
	INPUT_SCANCODE_BRIGHTNESSUP = SDL_SCANCODE_BRIGHTNESSUP,
	INPUT_SCANCODE_DISPLAYSWITCH = SDL_SCANCODE_DISPLAYSWITCH,
	INPUT_SCANCODE_KBDILLUMTOGGLE = SDL_SCANCODE_KBDILLUMTOGGLE,
	INPUT_SCANCODE_KBDILLUMDOWN = SDL_SCANCODE_KBDILLUMDOWN,
	INPUT_SCANCODE_KBDILLUMUP = SDL_SCANCODE_KBDILLUMUP,
	INPUT_SCANCODE_EJECT = SDL_SCANCODE_EJECT,
	INPUT_SCANCODE_SLEEP = SDL_SCANCODE_SLEEP
};

static bool HandleEvents(Input* input)
{
	SDL_Event event;
	InputResetMouseScroll(input);
	/* SDL_SetRelativeMouseMode(SDL_TRUE);*/
	
	for (int i = 0; i < INPUT_NUM_MOUSEBUTTONS; i++) {
		input->mouseButtonsDown[i] = 0;
		input->mouseButtonsUp[i] = 0;		
	}
	
	for (int i = 0; i < input->numKeys; i++) {
		input->keysDown[i] = 0;
		input->keysUp[i] = 0;		
	}
		
	i32 relx, rely;
	SDL_GetRelativeMouseState(&relx, &rely);
	InputSetRelativeMouseMotion(input, relx, rely);
	
	i32 x, y;
	SDL_GetMouseState(&x, &y);
	InputSetMousePosition(input, x, y);
	
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			return false;
		}
		if (event.type == SDL_KEYDOWN) {
			int scancode = event.key.keysym.scancode;
			input->keysDown[scancode] = 1;		
		}
		if (event.type == SDL_KEYUP) {
			int scancode = event.key.keysym.scancode;
			input->keysUp[scancode] = 1;	
		}
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			int button = event.button.button;
			input->mouseButtons[button] = 1;
			input->mouseButtonsDown[button] = 1;
		}
		if (event.type == SDL_MOUSEBUTTONUP) {
			int button = event.button.button;
			input->mouseButtons[button] = 0;
			input->mouseButtonsUp[button] = 1;
		}
		if (event.type == SDL_MOUSEWHEEL) {
			SDL_MouseWheelEvent* mwEvent = (SDL_MouseWheelEvent*) &event;
			InputAddMouseScroll(input, mwEvent->x, mwEvent->y);
		}
		if (event.type == SDL_CONTROLLERDEVICEADDED) {
			
		}
		if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
			
		}
		if (event.type == SDL_CONTROLLERDEVICEREMAPPED) {
			
		}
		if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				/* TODO */
			}
		}
	}
	
	return true;
}

static void AudioCallback(void* userData, u8* audioData, int length)
{
    /* Clear our audio buffer to silence. */
    memset(audioData, 0, length);
}


int main(int argc, char* argv[])
{
	// int width = 800;
	// int height = 600;
	// Window window = SDL2CreateWindow(
	// 		"TQ Engine", 
	// 		width, 
	// 		height,
	// 		0
	// 		//SDL_WINDOW_SHOWN |
    //         /*SDL_WINDOW_RESIZABLE |*/ 
    //         //SDL_RENDERER_PRESENTVSYNC |
    //         //SDL_WINDOW_INPUT_FOCUS
	// );
	
	if(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
		printf("SDL could not initialize window! SDL_Error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	SDL_Window* window = SDL_CreateWindow(
			"TQ Engine",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			800,
			600,
			0
	);
	
	VkInstance instance;
	{
		VkApplicationInfo appInfo;
		VkInstanceCreateInfo instanceCreateInfo;
		
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = "TQ Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = NULL;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.ppEnabledLayerNames = NULL;
	
		unsigned int extensionCount = 0;
		char** extensions = SDL_AllocVulkanInstanceExtensions(&extensionCount);
		instanceCreateInfo.enabledExtensionCount = extensionCount;
		instanceCreateInfo.ppEnabledExtensionNames = extensions;
		instanceCreateInfo.enabledLayerCount = 0;
	
		if (vkCreateInstance(&instanceCreateInfo, NULL, &instance) != VK_SUCCESS) {
			printf("Couldn't create VkInstance.\n");
			SDL_FreeVulkanInstanceExtensions(extensions);
			exit(EXIT_FAILURE);
		}
		
		SDL_FreeVulkanInstanceExtensions(extensions);
	}
	
	VkSurfaceKHR surface;
    if (!SDL_CreateVulkanSurface(instance, window, NULL, &surface)) {
        printf("SDL_CreateVulkanSurface failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	/* Update */
	
	/* Shutdown */
	vkDestroyInstance(instance, NULL);
	SDL_Quit();
	
	// Renderer renderer = CreateRenderer(width, height);
	
	// int numKeys;
	// u8* keyboardState = (u8*) SDL_GetKeyboardState(&numKeys);
	// Input input = CreateInput(keyboardState, numKeys);
	
	// if(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
	// 	printf("SDL could not initialize controller system! SDL_Error: %s\n", SDL_GetError());
	// 	exit(EXIT_FAILURE);
	// }
	
	// if(SDL_InitSubSystem(SDL_INIT_HAPTIC) != 0) {
	// 	printf("SDL could not initialize controller rumbling! SDL_Error: %s\n", SDL_GetError());
	// 	exit(EXIT_FAILURE);
	// }
	
	// const int numJoysticks = SDL_NumJoysticks();
	// const int maxControllers = 4;
	// SDL_GameController* controllers[4] = {NULL};
	// SDL_Haptic* rumbleHandles[4];
	
	// int controllerIndex = 0;
	// for(int i = 0; i < numJoysticks; i++) {
    // 	if (!SDL_IsGameController(i)) {
	// 		continue;
	// 	}
	// 	if (controllerIndex >= maxControllers) {
	// 		break;
	// 	}
	// 	controllers[controllerIndex] = SDL_GameControllerOpen(i);
		
	// 	SDL_Joystick* joystickHandle = SDL_GameControllerGetJoystick(controllers[controllerIndex]);
	// 	rumbleHandles[controllerIndex] = SDL_HapticOpenFromJoystick(joystickHandle);
	// 	if (SDL_HapticRumbleInit(rumbleHandles[controllerIndex]) != 0) {
	// 		SDL_HapticClose(rumbleHandles[controllerIndex]);
	// 		rumbleHandles[controllerIndex] = 0;
	// 	}
		
	// 	controllerIndex++;
	// }
	
	// if(SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
	// 	printf("SDL could not initialize audio system! SDL_Error: %s\n", SDL_GetError());
	// 	exit(EXIT_FAILURE);
	// }
	
	// SDL_AudioSpec audioSettings = {0};
	
	// audioSettings.freq = 44100;
	// audioSettings.format = AUDIO_S16LSB;
	// audioSettings.channels = 2;
	// audioSettings.samples = 10000;
	// audioSettings.callback = &AudioCallback;

	// SDL_OpenAudio(&audioSettings, 0);
	
	// bool isRunning = true;
	// float dt = 0.0f;
	// Clock clock = CreateClock(60);
	// StartClock(&clock);
	
	// while (isRunning)
	// {
	// 	dt = CalcClockDelta(&clock);
	// 	isRunning = HandleEvents(&input);
	// 	for (int i = 0; i < maxControllers; i++) {
	// 		if(controllers[i] != 0 && SDL_GameControllerGetAttached(controllers[i])) {
	// 			// u8 up = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_DPAD_UP);
	// 			// u8 down = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	// 			// u8 left = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	// 			// u8 right = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	// 			// u8 start = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_START);
	// 			// u8 guide = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_GUIDE);				
	// 			// u8 back = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_BACK);
	// 			// u8 leftShoulder = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
	// 			// u8 rightShoulder = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
	// 			u8 aButton = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_A);
	// 			u8 bButton = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_B);
	// 			// u8 xButton = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_X);
	// 			// u8 yButton = SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_Y);

	// 			// i16 stickX = SDL_GameControllerGetAxis(controllers[i], SDL_CONTROLLER_AXIS_LEFTX);
	// 			// i16 stickY = SDL_GameControllerGetAxis(controllers[i], SDL_CONTROLLER_AXIS_LEFTY);
				
	// 			if (aButton) {
	// 				printf("A pressed!\n");
	// 				if (rumbleHandles[i]) {
	// 					const float strength = 0.5f /* percentage in [0,1] */;
	// 					const u32 length = 500; /* in milliseconds */
	// 					SDL_HapticRumblePlay(rumbleHandles[i], strength, length);
	// 					//SDL_HapticRumbleStop(rumbleHandles[i]);
	// 				}
	// 			}
				
	// 			if (bButton) {
	// 				printf("B pressed!\n");
	// 			}
	// 		}
	// 	}
		
		
	// 	if (input.keys[INPUT_SCANCODE_W]) {
	// 		printf("Holding W\n");
	// 	}
		
	// 	if (input.keysUp[INPUT_SCANCODE_W]) {
	// 		printf("Released W\n");
	// 	}
		
	// 	if (input.mouseButtons[INPUT_MOUSE_BUTTON_LEFT]) {
	// 		printf("Holding left click\n");
	// 	}
		
	// 	if (input.mouseButtonsUp[INPUT_MOUSE_BUTTON_LEFT]) {
	// 		printf("Left click released\n");
	// 	}
		
	// 	while (IsClockAccumulating(&clock)) {
	// 		/* Fixed stuff */
	// 		AccumulateClock(&clock);
	// 	}
		
	// 	Color clearColor;
	// 	clearColor.r = 0;
	// 	clearColor.g = 0;
	// 	clearColor.b = 0;
	// 	clearColor.a = 255;
		
	// 	Color color;
	// 	color.r = 255;
	// 	color.g = 255;
	// 	color.b = 255;
	// 	color.a = 255;
		
	// 	Color color2;
	// 	color2.r = 0;
	// 	color2.g = 255;
	// 	color2.b = 0;
	// 	color2.a = 255;
		
		
	// 	ClearBackBuffer(&renderer, &clearColor);
	// 	DrawPixel(&renderer, 80, 40, &color2);
	// 	DrawTriangle(&renderer, 80, 100, 150, 200, 100, 300, &color);
	// 	DrawTriangle(&renderer, 300, 300, 150, 400, 450, 400, &color2);
		
	// 	SDL2SwapBuffers(&window, &renderer);
	// }
	
	// for (int i = 0; i < maxControllers; i++) {
	// 	if (controllers[i]) {
	// 		SDL_GameControllerClose(controllers[i]);
	// 	}
	// 	if (rumbleHandles[i]) {
	// 		SDL_HapticClose(rumbleHandles[i]);
	// 	}
	// }
	
	// DestroyInput(&input);
	// DestroyRenderer(&renderer);
	// SDL2DestroyWindow(&window);
	
	return 0;
}