#include "HelloTriangleApplication.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
// validation layers constants that will be useful
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else 
const bool enableValidationLayers = true;
#endif 

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

void HelloTriangleApplication::run() {
	this->initWindow();
	this->initVulkan();
	this->mainLoop();
	this->cleanup();
}

void HelloTriangleApplication::createInstance() {

	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("Validation layers requested but are not available on this machine.");
	}
	
	// specifying some extra information to the driver to optimize our application
	
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello triangle woah";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // this is actually so useful holy shit
	appInfo.pEngineName = "David Engine (biblically acccurate)";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	
	// we need to pass throw global extensions for window managementn glfw makes this a bit simpler for us
	
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	
	// keep this empty for right now
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {		
		createInfo.enabledLayerCount = 0;
	}
	
	// before we create the instance we first want to check to see what extensions are available to us

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	std::cout << "Accepted extensions" << std::endl;
	
	for (const auto& extension : extensions) { // remember that c++ has for-each loops now
		std::cout << "\t" << extension.extensionName << '\n'; // doing '\n' is actually better than endl since endl also flushes the stream
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) { // we can make this an error check function (hellloooooooo)
		throw std::runtime_error("failed to create instance!");
	}

}

void HelloTriangleApplication::initVulkan() {
	// making an instance, this is connection between vulkan and this application
	createInstance();

}


void HelloTriangleApplication::mainLoop() {
	// have an event loop that continues until eventually the window closes or an error is thrown
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents(); // there should be a way to also send out this signal ourselves without clicking the "x" button
	}




}

void HelloTriangleApplication::cleanup() {
	// destory the window and deallocate the memory
	glfwDestroyWindow(window);
	glfwTerminate();
	// make sure you destory the instance of vulkan that was created
	vkDestroyInstance(instance, nullptr); // no callbacks here but if we did have one we'd have to but in in this function for de allocation
}

// pretty self explanatory what this does
void HelloTriangleApplication::initWindow() {
	// first we initialize the glfw library and then immeditaely tell it not to you use opengl context
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "this is a title", nullptr, nullptr);
}

bool HelloTriangleApplication::checkValidationLayerSupport() {
	uint32_t validationCount = 0;
	vkEnumerateInstanceLayerProperties(&validationCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(validationCount);
	vkEnumerateInstanceLayerProperties(&validationCount, availableLayers.data());
	
	for (auto& validationLayer : validationLayers) {
		bool entryFlag = false;
		for (auto& available : availableLayers) {
			if (std::strcmp(available.layerName, validationLayer) == 0) {
				entryFlag = true;
				break;
			}	
		}
		if (!entryFlag) {
			return false;
		}
	}
	return true;
}