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



VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}

	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}





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
	
	auto glfwextensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwextensions.size());
	createInfo.ppEnabledExtensionNames = glfwextensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	// keep this empty for right now
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		this->populateMessengerCreate(&debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;

	}
	else {		
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
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
	setupDebugMessenger();
}


void HelloTriangleApplication::populateMessengerCreate(VkDebugUtilsMessengerCreateInfoEXT* createInfo) {
	createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo->pfnUserCallback = debugCallback;
	createInfo->pUserData = nullptr; // Optional

}

void HelloTriangleApplication::setupDebugMessenger() {
	if (!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	this->populateMessengerCreate(&createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up the debug messenger");
	}


}

void HelloTriangleApplication::mainLoop() {
	// have an event loop that continues until eventually the window closes or an error is thrown
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents(); // there should be a way to also send out this signal ourselves without clicking the "x" button
	}




}

void HelloTriangleApplication::cleanup() {
	if (enableValidationLayers == true) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	
	// make sure you destory the instance of vulkan that was created
	vkDestroyInstance(instance, nullptr); // no callbacks here but if we did have one we'd have to but in in this function for de allocation
	
	// destory the window and deallocate the memory
	glfwDestroyWindow(window);
	glfwTerminate();
	
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


std::vector<const char*>HelloTriangleApplication::getRequiredExtensions() {
	// I didn't realize that putting less comments actually makes stuff so much nicer to look at
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (enableValidationLayers) {
	
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}






