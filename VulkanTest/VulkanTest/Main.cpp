// drawing a simple triangle
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib> // c standard library


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
	// the instance made for vulkan must be destroyed
		vkDestroyInstance(instance, nullptr);
		
		
		// need to destroy the window we are rendering to and terminate glfw
		glfwDestroyWindow(appWindow);
		glfwTerminate();
	

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


