#pragma once

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif __linux__
#define VK_USE_PLATFORM_XCB_KHR
#elif __ANDROID__
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#include <string.h>
#include <sdl/SDL_syswm.h>
#include <sdl/SDL_video.h>
#include <vulkan/vulkan.h>

static SDL_bool SetNames(unsigned int* count, const char** names, unsigned int inCount, const char* const* inNames)
{
    unsigned int capacity = *count;
    unsigned int i;
    
    *count = inCount;
    if (names) {
        if (capacity < inCount) {
            SDL_SetError("Insufficient capacity for extension names: %u < %u", capacity, inCount);
            return SDL_FALSE;
        }
        for (i = 0; i < inCount; i++) {
            names[i] = inNames[i];
        }
    }
    return SDL_TRUE;
}

SDL_bool SDL_GetVulkanInstanceExtensions(unsigned int* count, const char** names)
{
    const char* driver = SDL_GetCurrentVideoDriver();
    if (!driver) {
        SDL_SetError("No video driver - has SDL_Init(SDL_INIT_VIDEO) been called?");
        return SDL_FALSE;
    }
    if (!count) {
        SDL_SetError("'count' is NULL");
        return SDL_FALSE;
    }
    
#ifdef _WIN32
    if(strcmp(driver, "windows") == 0) {
       const char* ext[] = { VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
       return SetNames(count, names, 1, ext);
    }
#elif __linux__
    if (strcmp(driver, "x11") == 0) {
        const char* ext[] = { VK_KHR_XCB_SURFACE_EXTENSION_NAME };
        return SetNames(count, names, 1, ext);
    }
#elif __ANDROID__
    if (strcmp(driver, "android") == 0) {
        const char* ext[] = { VK_KHR_ANDROID_SURFACE_EXTENSION_NAME };
        return SetNames(count, names, 1, ext);
    }
#endif
    (void) SetNames;
    (void) names;

    SDL_SetError("Unsupported video driver '%s'", driver);
    return SDL_FALSE;
}

SDL_bool SDL_CreateVulkanSurface(VkInstance instance, SDL_Window* window, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface)
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
            sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            sci.pNext = NULL;
            sci.flags = 0;
            sci.hinstance = GetModuleHandle(NULL);
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