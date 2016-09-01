#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sdl/SDL.h>
#include "sdl_vulkan.h"

#include "platform.h"
#include "file.h"
#include "input.h"
#include "vk_render.h"

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

/*
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
*/

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

static VKAPI_ATTR VkBool32 VKAPI_CALL 
VulkanDebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, 
	size_t location, int32_t code, const char* layerPrefix, 
	const char* msg, void* userData)
{
	printf("Validation layer: %s\n", msg);
	return VK_FALSE;
}


int main(int argc, char* argv[])
{	
	if(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
		printf("SDL could not initialize window! SDL_Error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	int windowWidth = 800;
	int windowHeight = 600;
	SDL_Window* window = SDL_CreateWindow(
			"TQ Engine",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			windowWidth,
			windowHeight,
			0
	);
	
	Input input;
	{
		int numKeys;
		u8* keyboardState = (u8*) SDL_GetKeyboardState(&numKeys);
		input = CreateInput(keyboardState, numKeys);
	}
	
	/* Create instance */
	VkInstance instance;
	{
		VkApplicationInfo appInfo;
		VkInstanceCreateInfo createInfo;
		
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = "TQ Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.ppEnabledLayerNames = NULL;
	
		unsigned int extensionCount = 0;
		char* extensions[SDLTQ_VULKAN_NUM_INSTANCE_EXTENSIONS];
		if (!SDLTQ_LoadVulkanInstanceExtensions(extensions, &extensionCount)) {
			printf("Error loading Vulkan Instance extensions: %s\n", SDL_GetError());			
			exit(EXIT_FAILURE);
		}
		createInfo.enabledExtensionCount = extensionCount;
		createInfo.ppEnabledExtensionNames = extensions;
		createInfo.enabledLayerCount = 0;
	
		if (vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS) {
			printf("Couldn't create VkInstance.\n");
			exit(EXIT_FAILURE);
		}
	}
	
	VkSurfaceKHR surface;
    if (!SDLTQ_CreateVulkanSurface(instance, window, NULL, &surface)) {
        printf("SDLTQ_CreateVulkanSurface failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	/* TODO: Create validation layers */
	VkDebugReportCallbackEXT debugCallback;
	{	
		bool enableValidationLayers = true;
		const char* validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
		VkDebugReportCallbackCreateInfoEXT createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.pNext = NULL;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        createInfo.pfnCallback = VulkanDebugCallback;
		createInfo.pUserData = NULL;
		/*
		PFN_vkCreateDebugReportCallbackEXT create = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
		if (!create) {
			printf("Vulkan renderer: Failed to set up debug callback!\n");
			exit(EXIT_FAILURE);
		}
		
		if (create(instance, &createInfo, NULL, &debugCallback) != VK_SUCCESS) {
			printf("Vulkan renderer: Failed to set up debug callback!\n");
			exit(EXIT_FAILURE);
		}*/
	}
	
	/* Create physical device */
	VkPhysicalDevice gpu;
    {
        uint32_t numGPUs;
        if (vkEnumeratePhysicalDevices(instance, &numGPUs, NULL) != VK_SUCCESS) {
			printf("Vulkan renderer: Can't enumerate GPU's.\n");
			exit(EXIT_FAILURE);
		}
		
		if (numGPUs > TQ_RENDERER_MAX_GPUS) {
			numGPUs = TQ_RENDERER_MAX_GPUS;
		}
		
        if (numGPUs > 0) {
			VkPhysicalDevice gpus[TQ_RENDERER_MAX_GPUS];
           	if (vkEnumeratePhysicalDevices(instance, &numGPUs, gpus) != VK_SUCCESS) {
				printf("Vulkan renderer: Can't enumerate GPU's.\n");
				exit(EXIT_FAILURE);   
			}
			/* Choose suitable GPU (here first GPU) */
            gpu = gpus[0];
			if (gpu == VK_NULL_HANDLE) {
    			printf("Vulkan renderer: Failed to find a suitable GPU.\n");
			}	
        }
	}
	
	/* Create device & queues */
	VkDevice device = VK_NULL_HANDLE;
	VkQueue presentationQueue;
	uint32_t presentationQueueIndex = -1;
	{	
		/* Check for Queue Families*/
		
		uint32_t numQueues = TQ_RENDERER_MAX_DEVICE_QUEUES;
        VkQueueFamilyProperties queueProperties[TQ_RENDERER_MAX_QUEUE_FAMILY_PROPERTIES];
		
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &numQueues, queueProperties);
        if (numQueues < 1) {
			printf("Vulkan renderer: Can't get GPU Queue Family Properties.\n");
			exit(EXIT_FAILURE);
		}

		/* Queue familiy for presentation (surface) & graphics */
		/* Might split those up and make API for multiple queues */
        for (uint32_t i = 0; i < numQueues; i++) {
        	VkBool32 presentationSupported;
            vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentationSupported);
            if (presentationSupported && (queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            	presentationQueueIndex = i;
            	break;
            }
        }
        if (presentationQueueIndex == -1) {
			printf("Vulkan Renderer: Can't get suitable GPU Queue Family.\n");
			exit(EXIT_FAILURE);
		}
		
		/* Only one queue & multiple command buffers per thread */
		/* Submit command buffers to queue on main thread */
		/* NOTE: Use list of queueCreateInfo's in future */
		VkDeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = NULL;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = presentationQueueIndex;
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f; /* for scheduling multiple queues */
		queueCreateInfo.pQueuePriorities = &queuePriority;
		
		VkPhysicalDeviceFeatures deviceFeatures = {VK_FALSE};
		VkDeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = NULL;
		deviceCreateInfo.flags = 0;
        deviceCreateInfo.queueCreateInfoCount = 1; /* Might change to list*/
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo; /* idem */
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.ppEnabledLayerNames = NULL;
    	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		
		/* Extensions */
		const uint32_t extensionCount = 1;
		const char* extensions[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; 
        deviceCreateInfo.enabledExtensionCount = extensionCount;
        deviceCreateInfo.ppEnabledExtensionNames = extensions;
		
		if (vkCreateDevice(gpu, &deviceCreateInfo, NULL, &device) != VK_SUCCESS) {
			printf("Vulkan renderer: Can't create logical device.\n");
			exit(EXIT_FAILURE);
		}
		
		/* Create the queues */
		vkGetDeviceQueue(device, presentationQueueIndex, 0, &presentationQueue);
	}
	
	/* Create swapchain */
	VkSwapchainKHR swapchain = NULL;
	VkImage* swapchainImages = NULL;
	uint32_t swapchainImageCount;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	{
		/* Surface capabilities and swap extent */
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		{
			if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfaceCapabilities) != VK_SUCCESS) {
				printf("Vulkan renderer: Can't get surface capabilities.\n");			
				exit(EXIT_FAILURE);
			}
			
			if (surfaceCapabilities.currentExtent.width == (uint32_t) -1) {
        	    swapchainExtent.width = (uint32_t) windowWidth;
        	    swapchainExtent.height = (uint32_t) windowHeight;
        	} else {
        	    swapchainExtent = surfaceCapabilities.currentExtent;			
        	}
		}
		
		/* Choose swapchain image format */
		VkSurfaceFormatKHR surfaceFormat;
		{			
			uint32_t formatCount;
			VkSurfaceFormatKHR formats[1] =
			{
				{.format = VK_FORMAT_MAX_ENUM, .colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR}
			};
        	if (vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, formats) != VK_SUCCESS) {
				printf("Vulkan renderer: Can't get surface formats.\n");			
				exit(EXIT_FAILURE);
			}
			
			/* TODO: might revisit this code? */
			if (formatCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        	    surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
				surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        	} else {
        	    surfaceFormat.format = formats[0].format;
        	    surfaceFormat.colorSpace = formats[0].colorSpace;			
        	}
			
			swapchainImageFormat = surfaceFormat.format;
		}
		
		/* Choose present mode */
		VkPresentModeKHR presentMode;
		{	
			uint32_t presentModeCount;
			VkPresentModeKHR presentModes[] = { VK_PRESENT_MODE_MAX_ENUM_KHR };
			if (vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, presentModes) != VK_SUCCESS) {
				printf("Vulkan renderer: Can't get surface present modes.\n");
				exit(EXIT_FAILURE);
			}
			
			/* Choose triple buffering if it's available */
			for (uint32_t i = 0; i < presentModeCount; i++) {
				if (presentModes[i] = VK_PRESENT_MODE_MAILBOX_KHR) {
					presentMode = presentModes[i];
					break;
				}
			}
		}
		
		/* Create swapchain */
		VkSwapchainCreateInfoKHR createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.pNext = NULL;
		createInfo.flags = 0;		
		createInfo.surface = surface;
		
		swapchainImageCount = surfaceCapabilities.minImageCount;
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			swapchainImageCount++;
		}
        if ((surfaceCapabilities.maxImageCount > 0) && (swapchainImageCount > surfaceCapabilities.maxImageCount)) {
            swapchainImageCount = surfaceCapabilities.maxImageCount;
        }
		
		createInfo.minImageCount = swapchainImageCount;
		createInfo.imageFormat = surfaceFormat.format;		
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = swapchainExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = NULL;
		createInfo.preTransform = surfaceCapabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		
		if (vkCreateSwapchainKHR(device, &createInfo, NULL, &swapchain) != VK_SUCCESS) {
			printf("Vulkan renderer: Failed to create swapchain.\n");
			exit(EXIT_FAILURE);			
		}
		
		/* Retrieve swapchain images */
		/* These are automatically freed when destroying swapchain */	
		if (vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL) != VK_SUCCESS) {
			printf("Vulkan renderer: Failed to retrieve swapchain images.\n");	
			exit(EXIT_FAILURE);			
		}
		swapchainImages = (VkImage*) malloc(swapchainImageCount * sizeof(VkImage));
		
		if (vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages) != VK_SUCCESS) {
			printf("Vulkan renderer: Failed to retrieve swapchain images.\n");
			exit(EXIT_FAILURE);								
		}
	}
		
	/* Create image views for swapchain images */
	VkImageView* swapchainImageViews;
	const uint32_t swapchainImageViewCount = swapchainImageCount;	
	{
		swapchainImageViews = (VkImageView*) malloc(swapchainImageViewCount * sizeof(VkImageView));	
		for (size_t i = 0; i < swapchainImageViewCount; i++) {
			VkImageViewCreateInfo createInfo;
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.pNext = NULL;
			createInfo.flags = 0;
			createInfo.image = swapchainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapchainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			
			if (vkCreateImageView(device, &createInfo, NULL, &swapchainImageViews[i]) != VK_SUCCESS) {
				printf("Vulkan renderer: Failed to create image views.\n");
				exit(EXIT_FAILURE);								
			}
		}
	}
	
	/* Create render pass */
	VkRenderPass renderPass;
	{
		VkAttachmentDescription colorAttachment;
		colorAttachment.flags = 0;
		colorAttachment.format = swapchainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		
		VkAttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		VkSubpassDescription subPass;
		subPass.flags = 0;
		subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subPass.inputAttachmentCount = 0;
		subPass.pInputAttachments = NULL;
		subPass.colorAttachmentCount = 1;
		subPass.pColorAttachments = &colorAttachmentRef;
		subPass.pResolveAttachments = NULL;
		subPass.pDepthStencilAttachment = NULL;
		subPass.preserveAttachmentCount = 0;
		subPass.pPreserveAttachments = NULL;
		
		VkRenderPassCreateInfo renderPassInfo;
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.pNext = NULL;
		renderPassInfo.flags = 0;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subPass;
		renderPassInfo.dependencyCount = 0;
		renderPassInfo.pDependencies = NULL;
		
		if (vkCreateRenderPass(device, &renderPassInfo, NULL, &renderPass) != VK_SUCCESS) {
            printf("Vulkan renderer: Failed to create render pass.\n");
			exit(EXIT_FAILURE);
        }
	}
	
	/* Create framebuffers */
	VkFramebuffer* swapchainFramebuffers;
	const uint32_t swapchainFramebufferCount = swapchainImageCount;
	{
		swapchainFramebuffers = (VkFramebuffer*) malloc(swapchainFramebufferCount * sizeof(VkFramebuffer));	
		for (uint32_t i = 0; i < swapchainFramebufferCount; i++) {
			VkFramebufferCreateInfo createInfo;
    		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.pNext = NULL;
			createInfo.flags = 0;
    		createInfo.renderPass = renderPass;
    		createInfo.attachmentCount = 1;
    		createInfo.pAttachments = swapchainImageViews;
    		createInfo.width = swapchainExtent.width;
    		createInfo.height = swapchainExtent.height;
    		createInfo.layers = 1;

    		if (vkCreateFramebuffer(device, &createInfo, NULL, &swapchainFramebuffers[i]) != VK_SUCCESS) {
				printf("Vulkan renderer: Failed to create framebuffer.\n");
				exit(EXIT_FAILURE);
    		}
		}
	}
	
	/* Create graphics pipeline */		
	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;
	VkShaderModule vertexShader = VK_NULL_HANDLE;
	VkShaderModule fragmentShader = VK_NULL_HANDLE;	
	char* vertCode;
	char* fragCode;
	{
		/* Programmable functions */
		
		/* Vertex shader */
		long vertCodeSize = 0;
		vertCode = tqReadFile("../data/shaders/bin/basic-vert.spv", &vertCodeSize);
		VkShaderModuleCreateInfo vertShaderCreateInfo;
		vertShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertShaderCreateInfo.pNext = NULL;
		vertShaderCreateInfo.flags = 0;		
		vertShaderCreateInfo.codeSize = vertCodeSize;
		vertShaderCreateInfo.pCode = (uint32_t*) vertCode;

		if (vkCreateShaderModule(device, &vertShaderCreateInfo, NULL, &vertexShader) != VK_SUCCESS) {
    		printf("Vulkan renderer: Failed to create shader module!\n");
			exit(EXIT_FAILURE);
		}
		
		/* Fragment shader */
		long fragCodeSize = 0;
		fragCode = tqReadFile("../data/shaders/bin/basic-frag.spv", &fragCodeSize);
		VkShaderModuleCreateInfo fragShaderCreateInfo;
		fragShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragShaderCreateInfo.pNext = NULL;
		fragShaderCreateInfo.flags = 0;		
		fragShaderCreateInfo.codeSize = fragCodeSize;
		fragShaderCreateInfo.pCode = (uint32_t*) fragCode;

		if (vkCreateShaderModule(device, &fragShaderCreateInfo, NULL, &fragmentShader) != VK_SUCCESS) {
    		printf("Vulkan renderer: Failed to create shader module!\n");
			exit(EXIT_FAILURE);
		}
		
		VkPipelineShaderStageCreateInfo vertShaderStageInfo;
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.pNext = NULL;
		vertShaderStageInfo.flags = 0;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertexShader;
		vertShaderStageInfo.pName = "main";
		vertShaderStageInfo.pSpecializationInfo = NULL;	
		
		VkPipelineShaderStageCreateInfo fragShaderStageInfo;
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.pNext = NULL;
		fragShaderStageInfo.flags = 0;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragmentShader;
		fragShaderStageInfo.pName = "main";
		fragShaderStageInfo.pSpecializationInfo = NULL;
	
		VkPipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0] = vertShaderStageInfo;
		shaderStages[1] = fragShaderStageInfo;
		
		/* Fixed functions */
		
		/* Vertex input */
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.pNext = NULL;
		vertexInputInfo.flags = 0;
		/* TODO: Vertex data */
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = NULL;
		vertexInputInfo.pVertexAttributeDescriptions = NULL;
		
		/* Input assembly: just draw triangles here */
		VkPipelineInputAssemblyStateCreateInfo inputAssembly;
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.pNext = NULL;
		inputAssembly.flags = 0;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		
		/* Viewport state */
		VkPipelineViewportStateCreateInfo viewportState;
        {
			VkViewport viewport;
        	viewport.x = 0.0f;
        	viewport.y = 0.0f;
        	viewport.width = (float) swapchainExtent.width;
        	viewport.height = (float) swapchainExtent.height;
        	viewport.minDepth = 0.0f;
        	viewport.maxDepth = 1.0f;
		
			VkRect2D scissor;
        	scissor.offset.x = 0;
			scissor.offset.y = 0;
        	scissor.extent = swapchainExtent;
			
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.pNext = NULL;
			viewportState.flags = 0;			
			viewportState.viewportCount = 1;
			viewportState.pViewports = &viewport;
			viewportState.scissorCount = 1;
			viewportState.pScissors = &scissor;
		}
		
		/* Rasterizer */
		VkPipelineRasterizationStateCreateInfo rasterizer;
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.pNext = NULL;
		rasterizer.flags = 0;
		rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;
        rasterizer.lineWidth = 1.0f;
		
		/* Anti-aliasing (MSAA) */
		VkPipelineMultisampleStateCreateInfo multisampling;
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.pNext = NULL;
		multisampling.flags = 0;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = NULL;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;
		
		/* TODO: Depth & stencil tests */
		/* ... */
		
		/* TODO: Color blending */
		/* 
			finalColor.rgb = alpha * newColor + (1 - alpha) * oldColor;
			finalColor.a = alpha;
		*/
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		
		VkPipelineColorBlendStateCreateInfo colorBlending;
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
		
		/* TODO (in near future): Dynamic state */
		VkPipelineDynamicStateCreateInfo dynamicState;
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.pNext = NULL;
		dynamicState.flags = 0;
		dynamicState.dynamicStateCount = 0;
		dynamicState.pDynamicStates = NULL;
		
		/* Pipeline layout (for uniforms in shaders) */
		VkPipelineLayoutCreateInfo pipelineLayoutInfo;
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pNext = NULL;
		pipelineLayoutInfo.flags = 0;		
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = NULL;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = NULL;
		
		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
			printf("Vulkan renderer: Failed to create pipeline layout!\n");
			exit(EXIT_FAILURE);
		}
		
		/* Create graphics pipeline */
		VkGraphicsPipelineCreateInfo pipelineInfo;
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = NULL;
		pipelineInfo.flags = 0;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pTessellationState = NULL;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = NULL;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		/* Derive from existing pipeline (faster switching) */
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;
		
		/* Create all graphics pipelines (if there are many) */
		/* Might use pipeline cache */
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline) != VK_SUCCESS) {
    		printf("Vulkan renderer: Failed to create graphics pipeline!\n");
			exit(EXIT_FAILURE);			
		}
	}
	
	/* Create command pool */
	VkCommandPool commandPool;
	{
		VkCommandPoolCreateInfo createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.flags = 0; /* VkCommandPoolCreateFlagBits */
		createInfo.queueFamilyIndex = presentationQueueIndex;
		if (vkCreateCommandPool(device, &createInfo, NULL, &commandPool) != VK_SUCCESS) {
			printf("Vulkan renderer: Failed to create command pool.\n");
			exit(EXIT_FAILURE);
		}
	}
	
	/* Allocate & record command buffers */
	VkCommandBuffer* commandBuffers;
	const uint32_t commandBufferCount = swapchainImageCount;
	{
		commandBuffers = (VkCommandBuffer*) malloc(commandBufferCount * sizeof(VkCommandBuffer));
		VkCommandBufferAllocateInfo allocInfo;
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.pNext = NULL;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = commandBufferCount;
		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers) != VK_SUCCESS) {
			printf("Vulkan renderer: Failed to allocate command buffers.\n");
			exit(EXIT_FAILURE);
		}
		
		for (size_t i = 0; i < commandBufferCount; i++) {
    		VkCommandBufferBeginInfo beginInfo;
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = NULL;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = NULL;
			vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
			
			VkRenderPassBeginInfo renderPassInfo;
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.pNext = NULL;
			renderPassInfo.renderPass = renderPass;
			renderPassInfo.framebuffer = swapchainFramebuffers[i];
			renderPassInfo.renderArea.offset.x = 0;
			renderPassInfo.renderArea.offset.y = 0;			
			renderPassInfo.renderArea.extent = swapchainExtent;
			
			VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;
			
			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
			vkCmdEndRenderPass(commandBuffers[i]);
			
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				printf("Vulkan renderer: Failed to record command buffer!\n");
			}
		}
	}
		
	/* Update */
	{
		bool isRunning = true;
		float dt = 0.0f;
		Clock clock = CreateClock(60);
		StartClock(&clock);
		
		while (isRunning) {
			dt = CalcClockDelta(&clock);
			isRunning = HandleEvents(&input);
		}
	}
	
	/* Shutdown */
	{
		vkDeviceWaitIdle(device);
		
		vkFreeCommandBuffers(device, commandPool, commandBufferCount, commandBuffers);
		free(commandBuffers);
		vkDestroyCommandPool(device, commandPool, NULL);
		vkDestroyPipeline(device, graphicsPipeline, NULL);
		vkDestroyPipelineLayout(device, pipelineLayout, NULL);
		vkDestroyShaderModule(device, fragmentShader, NULL);
		free(fragCode);
		vkDestroyShaderModule(device, vertexShader, NULL);
		free(vertCode);
		for (uint32_t i = 0; i < swapchainFramebufferCount; i++) {
			vkDestroyFramebuffer(device, swapchainFramebuffers[i], NULL);
		}
		free(swapchainFramebuffers);
		vkDestroyRenderPass(device, renderPass, NULL);
		for (uint32_t i = 0; i < swapchainImageViewCount; i++) {
			vkDestroyImageView(device, swapchainImageViews[i], NULL);
		}
		free(swapchainImageViews);
		vkDestroySwapchainKHR(device, swapchain, NULL);
		vkDestroyDevice(device, NULL);
		vkDestroySurfaceKHR(instance, surface, NULL);
		vkDestroyInstance(instance, NULL);
	
		DestroyInput(&input);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
	
	return 0;
}
	
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
	
	// return 0;
// }