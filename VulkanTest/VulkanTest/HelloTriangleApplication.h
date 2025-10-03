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
    std::optional<uint32_t> presentFamily;
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    
};


class HelloTriangleApplication {
public:
    void run();

    std::vector<const char*> getRequiredExtensions();

    VkDevice device;

private:
    
    // members
    GLFWwindow* window;

    VkQueue graphicsQueue;

    VkQueue presentQueue;
    
    VkInstance instance;

    VkDebugUtilsMessengerEXT debugMessenger;

    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkSwapchainKHR swapChain;


    // methods
    void initWindow();

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    
    void initVulkan();

    void setupDebugMessenger();


    VkSurfaceFormatKHR chooseSwapFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availableModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void createSwapChain();
    
    void mainLoop();

    void cleanup();

    void createInstance();

    void createSurface();

    bool checkValidationLayerSupport();

    void populateMessengerCreate(VkDebugUtilsMessengerCreateInfoEXT* createInfo);

    void pickPhysicalDevice();

    int getDeviceScore(VkPhysicalDevice device);

    void createLogicalDevice();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    

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