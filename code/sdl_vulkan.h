#pragma once

/*
    NOTE: Change include paths below if necessary!
          Also, functions are prefixed SDLTQ_ to make
          sure it doesn't clash with future SDL intentions.
    
    Usage code:
    
    1) Creating a VkInstance:
    
    VkInstance instance;    
    ...
    VkInstanceCreateInfo instanceCreateInfo;
    ...
    unsigned int extensionCount = 0;
->  char** extensions = SDLTQ_AllocVulkanInstanceExtensions(&extensionCount);
    instanceCreateInfo.enabledExtensionCount = extensionCount;
	instanceCreateInfo.ppEnabledExtensionNames = extensions;
    
    if (vkCreateInstance(&instanceCreateInfo, NULL, &instance) != VK_SUCCESS) {
	    printf("Couldn't create VkInstance.\n");
->		SDLTQ_FreeVulkanInstanceExtensions(extensions);
		exit(EXIT_FAILURE);
	}
		
->  SDLTQ_FreeVulkanInstanceExtensions(extensions)
    
    2) Creating a VkSurfaceKHR:
    
    VkSurfaceKHR surface;
->  if (!SDLTQ_CreateVulkanSurface(instance, window, NULL, &surface)) {
        printf("SDLTQ_CreateVulkanSurface failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
*/

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif __linux__
#define VK_USE_PLATFORM_XCB_KHR
#elif __ANDROID__
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#include <stdlib.h>
#include <string.h>

/* Change include paths if necessary */
#include <sdl/SDL_video.h>
#include <vulkan/vulkan.h>
#include <sdl/SDL_syswm.h>

char** SDLTQ_AllocVulkanInstanceExtensions(unsigned int* count)
{
    const char* driver = SDL_GetCurrentVideoDriver();
    const unsigned int nrExtensions = 2;
    
    if (!driver) {
        SDL_SetError("No video driver - has SDL_Init(SDL_INIT_VIDEO) been called?");
        return NULL;
    }
    
    if (!count) {
        SDL_SetError("'count' is NULL");
        return NULL;
    }
    
    *count = nrExtensions;
    char** extensions = (char**) malloc(nrExtensions * sizeof(char*));    
    
#ifdef _WIN32
    if(strcmp(driver, "windows") == 0) {
        extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
        extensions[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
        return extensions;
    }
#elif __linux__
    if (strcmp(driver, "x11") == 0) {
        extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
        extensions[1] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
        return extensions;
    }
#elif __ANDROID__
    if (strcmp(driver, "android") == 0) {
        extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
        extensions[1] = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
        return extensions;
    }
#endif

    SDL_SetError("Unsupported video driver '%s'", driver);
    return NULL;
}

void SDLTQ_FreeVulkanInstanceExtensions(char** extensions)
{
    if (!extensions) {
        return;
    }
    free(extensions);
}

SDL_bool SDLTQ_CreateVulkanSurface(VkInstance instance, SDL_Window* window, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface)
{
    VkResult err;
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR sci;
#elif __linux__
    VkXcbSurfaceCreateInfoKHR sci;
#elif __ANDROID__
    VkAndroidSurfaceCreateInfoKHR sci;
#endif
    
    if (!window) {
        SDL_SetError("'window' is NULL");        
        return SDL_FALSE;
    }
    if (instance == VK_NULL_HANDLE) {
        SDL_SetError("'instance' is NULL");        
        return SDL_FALSE;
    }
    
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    if (!SDL_GetWindowWMInfo(window, &wminfo)) {
        return SDL_FALSE;
    }
    
    switch (wminfo.subsystem)
    {
#ifdef _WIN32
        case SDL_SYSWM_WINDOWS:
        {
            TCHAR* className = NULL;
            GetClassNameA(wminfo.info.win.window, className, 256);
            WNDCLASS wce;
            GetClassInfo(GetModuleHandle(NULL), className, &wce);
            sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            sci.pNext = NULL;
            sci.flags = 0;
            sci.hinstance = wce.hInstance;
            sci.hwnd = wminfo.info.win.window;
            err = vkCreateWin32SurfaceKHR(instance, &sci, allocator, surface);
        } break;
#elif __linux__        
        case SDL_SYSWM_X11:
        {
            sci.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
            sci.pNext = NULL;
            sci.flags = 0;
            sci.connection = XGetXCBConnection(wminfo.info.x11.display);
            sci.window = wminfo.info.x11.window;
            err = vkCreateXcbSurfaceKHR(instance, &sci, allocator, &surface);  
        } break;
#elif __ANDROID__
        case SDL_SYSWM_ANDROID:
        {
            sci.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
            sci.pNext = NULL;
            sci.flags = 0;
            sci.window = wminfo.info.android.window;
            err = vkCreateAndroidSurfaceKHR(instance, &sci, allocator, surface);
        } break;
#else
        default:
        { 
            SDL_SetError("Vulkan is not supported on this platform.");
            return SDL_FALSE;   
        } break;
#endif
    }
    
    return SDL_TRUE;
}