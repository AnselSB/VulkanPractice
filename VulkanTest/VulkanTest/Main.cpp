// drawing a simple triangle
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib> // c standard library
#include <vector> // pretty much always wanna do this graphics
#include <cstring>



const uint32_t WIDTH = 800; 
const uint32_t HEIGHT = 600;

// define a couple of configuration variables and specify the layers to enable and when they should be enabled 
// these are standard diagnostic layers provided by the vulkan sdk
const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};


// activates valdiation layers if the program is compiled in debug mode
#ifdef NDEBUG
	const bool enableValidationLayers = false; 
#else 
	const bool enableValidationLayers = true; 
#endif
// make a class that will act as a way to draw a triangle
class TriangleApp {

	// public methods
public:
	// pretty self explanatory
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	GLFWwindow* appWindow;
	VkInstance instance;
	// these functions are pretty self explanatory in what they do
	void initWindow() {
		// most functions seem to be camelCase
		glfwInit();
		// tell glfw not to make an opengl context
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		// create the actual window now, don't seem to need a monitor or share for right now so we can just set them to be null, the share is only an opengl thing so always leave it null 
		appWindow = glfwCreateWindow(800, 600, "tester", nullptr, nullptr);

	}
	void createInstance() {
		// check for validation layers first
		std::cout << checkValidationLayerSupport() << std::endl; 
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			// if validations is enabled but we don't have the support for it we need to throw an error
			throw std::runtime_error("Validation Layers request, but not available"); 
		
		}

		// make a struct that will specify some application information 
		// need to specify the s type in this struct, 
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; 
		appInfo.pApplicationName = "Draw Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); 
		appInfo.pEngineName = "No Engine"; //lmao we don't need engines where we're going
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); 
		appInfo.apiVersion = VK_API_VERSION_1_0; 
		
		// this struct has a pnext field but we're just gonna leave it as a nullptr since w edon't need it right now
		appInfo.pNext = nullptr; 


		// the next struct is not optiuonal unlike the one from before this tells vulkan srive rwhich global extension and validation layers will be used 
		// global as in whole program 
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; 
		createInfo.pApplicationInfo = &appInfo; // this can be null if you want, its the struct we just made a couple lines up
		// can think of these structs as objects that are used to pass parameters to the driver 
		// get the global extensions for the window manager we're using, which in this case is glfw
		uint32_t glfwExtensionCount = 0; 
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		// validation layer stuff, get more into this later
		createInfo.enabledLayerCount = 0;
		
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data(); 

		}
		else {
			createInfo.enabledLayerCount = 0; 
		}
		
		// use the get extensions in this fucntion to create a callstack		
		std::vector<const char*> extensions = getRequiredExtensions(); 
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size()); 
		createInfo.ppEnabledExtensionNames = extensions.data(); 
		
		// everything has been specified so now we to create an istance and issue tat call that will do that for us 
		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); 
		/*
		General pattern to object creation functions in vulkan: 
			- pointer to struct with creation info
			- pointer to custom allocator callbacks, nullptr in a lot of cases 
			- pointer to variable that will store the address to the object that was created 
		
		*/
		if (result != VK_SUCCESS) {
			throw new std::runtime_error("Failed to create an instance.");
		}

		std::cout << "Instance has been made" << std::endl; 
	}

	// this will demonstarte what a debug callback looks like 
	/*
	breakdown of the parameters: 
		- first guy: specifies severity of the message, enum type for specific values look at the documentation, these severities can be compared in an if statement 
		- message type: also enum type look at the documentation 
		- pCallbackData: struct that contains details of the message itself, has message, vulkan objects related to error, objectCount
		- pUserData: pointer that is specified during setup of callback and allows for me to pass my own data into it 
		- 
	*/
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT  messageSeverity, 
		VkDebugUtilsMessageTypeFlagsEXT messageType, 
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
		void * pUserData){
		
		
		std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE; 
		
	}


	void initVulkan() {
	// when initializing vulkan you need to create an instance, this creates a connection between application and the vulkan library, creating an instance involves specifying aspects about the appication to the drivers
		createInstance();

	}

	void mainLoop() {
	
		while (!glfwWindowShouldClose(appWindow)) {
			// this will become useful later when we need to define callback events for i/o (similar to what we did in cpsc 453)
			glfwPollEvents();
		}


	}

	void cleanup() {
	// the instance made for vulkan must be destroyed, nullptr parameter is optional, only for custom allocators, which to be honest we probably won't have to deal with 
		vkDestroyInstance(instance, nullptr);
		
		
		// need to destroy the window we are rendering to and terminate glfw
		glfwDestroyWindow(appWindow);
		glfwTerminate();
	}

	// pretty self explantory, checks to see if the requested layers are available 
	bool checkValidationLayerSupport() {
		// get number of layers 
		uint32_t layerCount; 
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount); 
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()); 
		// check to see if the string in validationLayers is in the available layers 
		for (const char* layer : validationLayers) {
			bool isValid = false; 
			for (const auto &available : availableLayers) {
				if(strcmp(available.layerName, layer) == 0) {
					isValid = true; 
				}
			}
			if (!isValid) {
				return false; 
			}
		
		}
		return true; 


	}

	// this will set up a message callback for when validation layer errors are triggered 
	// ensures that we only see the errors that we care for
	std::vector<const char*> getRequiredExtensions() {
		// this number may change based on the number of extension swe choose to add in the future
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		
		// make a vector of the extensions we have and make the size slightly bigger than what we need 
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		// extensions in glfw are always required but the debug messenger is conditionally added (we don't care to see it in prod)
		// pretty much push back onto the queue so that we can look at the callstack when something goes wrong 
		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		return extensions; 
	}
};


int main() {

	// initialize an instance of the app 
	TriangleApp app;
	try {
		app.run(); // this handles a lot of the logic within the object itself, it's really not too bad the funcitons that it calls on the other hand...
	} 
	// look into how try catch works in c++, never had to use it before, same as every other try catch to be quite honest
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}





	return EXIT_SUCCESS;
}


