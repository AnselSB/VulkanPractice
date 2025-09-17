#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

class HelloTriangleApplication {
public:
    void run();

private:
    
    GLFWwindow* window;
    
    void initWindow();
    
    void initVulkan();

    void mainLoop();

    void cleanup();

    VkInstance instance;

    void createInstance();

    bool checkValidationLayerSupport();
};