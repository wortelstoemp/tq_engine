#include <stdlib.h>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main(void)
{
    /* Init */
    GLFWwindow* window;
    const int width = 800;
    const int height = 600;
    
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        
    window = glfwCreateWindow(width, height, "TQ Engine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    /* Vulkan init stuff */
    VkInstance instance;
	
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = "TQ Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "TQ Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo instanceCreateInfo;
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = NULL;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.ppEnabledLayerNames = NULL;
	
	unsigned int extensionCount = 0;
	const char** extensionNames = NULL;
    extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
	printf("count: %u\n", extensionCount);
    
	instanceCreateInfo.enabledExtensionCount = extensionCount;
	instanceCreateInfo.ppEnabledExtensionNames = extensionNames;
	instanceCreateInfo.enabledLayerCount = 0;
	
	if (vkCreateInstance(&instanceCreateInfo, NULL, &instance) != VK_SUCCESS) {
		printf("Couldn't create VkInstance.\n");
	}
    
    /* Update */
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    
    /* Shutdown */
    vkDestroyInstance(instance, NULL);
    
    return 0;
}