#pragma once

#include <vulkan/vulkan.h>

#define TQ_RENDERER_MAX_GPUS 4
#define TQ_RENDERER_MAX_DEVICE_QUEUES 1
#define TQ_RENDERER_MAX_QUEUE_FAMILY_PROPERTIES (TQ_RENDERER_MAX_DEVICE_QUEUES * sizeof(VkQueueFamilyProperties))