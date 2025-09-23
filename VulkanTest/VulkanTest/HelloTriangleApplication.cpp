#include "HelloTriangleApplication.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <map>
#include <optional>

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
	pickPhysicalDevice();
	createLogicalDevice();
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

int HelloTriangleApplication::getDeviceScore(VkPhysicalDevice device) {
	// get the device properties
	VkPhysicalDeviceProperties deviceProps;
	VkPhysicalDeviceFeatures deviceFeats;
	vkGetPhysicalDeviceProperties(device, &deviceProps);
	vkGetPhysicalDeviceFeatures(device, &deviceFeats);
	
	int score = 0;
	// discrete gpus are goated so we should weight them highly
	if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	score += deviceProps.limits.maxImageDimension2D;

	// need geometry shader support for now
	if (!deviceFeats.geometryShader) {
		return 0;
	}
	if (!findQueueFamilies(device).isComplete()) {
		return 0;
	}

	return score;
}


void HelloTriangleApplication::pickPhysicalDevice() {

	// time to query for the number of available devices, I think eventually I wanna extend this out so that a user can pick which gpu they want to use (may not be necessary tho)
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	// must have at least one device otherwise nothing will run lmao
	if (deviceCount == 0) {
		throw std::runtime_error("No valid gpu's were found");
	}

	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	// remember data returns the direct pointer, which means it's returning the underlying array of the vector
	// because in c array == pointer
	vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
	
	std::multimap<int, VkPhysicalDevice>candidates;
	for (auto device : physicalDevices) {
		candidates.insert(std::make_pair(getDeviceScore(device), device));
	}
	if (candidates.rbegin()->first > 0) {
		physicalDevice = candidates.rbegin()->second;
	}
	else {
		throw std::runtime_error("unable to find a suitable gpu device");
	}

}

void HelloTriangleApplication::createLogicalDevice() {

	
}

QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.data());

	int i = 0;
	for (const auto &family : families) {
	
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		if (indices.isComplete()) {
			break;
		}
		i++;
	}

	return indices;



}






