#pragma once

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif __linux__
#define VK_USE_PLATFORM_XCB_KHR
#elif __ANDROID__
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#include <sdl/SDL_syswm.h>
#include <sdl/SDL_video.h>
#include <vulkan/vulkan.h>

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
        return SDL_FALSE;
    }
    if (instance == VK_NULL_HANDLE) {
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