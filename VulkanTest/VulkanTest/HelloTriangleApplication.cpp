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
#include <set>
#include <cstdint> 
#include <limits> 
#include <algorithm> 
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

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

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
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
}


void HelloTriangleApplication::createSurface() {
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface (uhb oh that's probably really fucking bad)");
	}



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

	vkDestroySwapchainKHR(device, swapChain, nullptr);

	vkDestroyDevice(device, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr); // this needs to be destroyed before the vulkan instance or things will go wrong

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

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties>availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// to now check to see if we have the proper extensions make a copy of the extension vector into a set
	// remove the names of all available extensions from the set
	// if the required ones are in the available then the required set will be empty
	// this is an algorithm for determining subsets I just realized
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}


	return requiredExtensions.empty();
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

	QueueFamilyIndices indices = findQueueFamilies(device);
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	// need geometry shader support for now
	if (!deviceFeats.geometryShader) {
		return 0;
	}
	if (!indices.isComplete() && !extensionsSupported) {
		return 0;
	}
	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	if (!swapChainAdequate) {
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

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.data());

	int i = 0;
	for (const auto& family : families) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (presentSupport) {
			indices.presentFamily = i;
		}
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

SwapChainSupportDetails HelloTriangleApplication::querySwapChainSupport(VkPhysicalDevice device) {
	// start by declaring the details object we are going to generate
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());

	}

	uint32_t presentCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, nullptr);

	if (presentCount != 0) {
		details.presentModes.resize(presentCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR HelloTriangleApplication::chooseSwapFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& format : availableFormats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}

	return availableFormats[0];
}


VkPresentModeKHR HelloTriangleApplication::choosePresentMode(const std::vector<VkPresentModeKHR>& availableModes) {
	for (const auto& mode : availableModes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return mode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}


VkExtent2D HelloTriangleApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	VkExtent2D actualExtent = {
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
	};


	actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	return actualExtent;

}


void HelloTriangleApplication::createSwapChain() {
	SwapChainSupportDetails chainSupport = querySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFmt = chooseSwapFormat(chainSupport.formats);
	VkPresentModeKHR presentMode = choosePresentMode(chainSupport.presentModes);
	VkExtent2D windowExtent = chooseSwapExtent(chainSupport.capabilities);

	uint32_t imageCount = std::clamp(chainSupport.capabilities.minImageCount + 1, chainSupport.capabilities.minImageCount, chainSupport.capabilities.maxImageCount);

	// time to fill out some structs (whoooooooo)
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFmt.format;
	createInfo.imageColorSpace = surfaceFmt.colorSpace;
	createInfo.imageExtent = windowExtent;
	createInfo.imageArrayLayers = 1; // pretty much a constant only change ig you're doing ar/vr for some reason
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // for post processing use VK_IMAGE_USAGE_TRANSFER_DST_BIT where we render on a separate image first

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = chainSupport.capabilities.currentTransform; // incase we want to apply linear transformation to the image itself
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create the swap chain!");
	}
}



