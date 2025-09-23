#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    
    bool isComplete() {
        return graphicsFamily.has_value();
    }
};


class HelloTriangleApplication {
public:
    void run();

    std::vector<const char*> getRequiredExtensions();

    VkDevice device;

private:
    
    GLFWwindow* window;
    
    VkInstance instance;

    VkDebugUtilsMessengerEXT debugMessenger;

    void initWindow();
    
    void initVulkan();

    void setupDebugMessenger();

    void mainLoop();

    void cleanup();

    void createInstance();

    bool checkValidationLayerSupport();

    void populateMessengerCreate(VkDebugUtilsMessengerCreateInfoEXT* createInfo);

    void pickPhysicalDevice();

    int getDeviceScore(VkPhysicalDevice device);

    void createLogicalDevice();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; 

    // I will have to look over this another time, I am too tired
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;

    }
};