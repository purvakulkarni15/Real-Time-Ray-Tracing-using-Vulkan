#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

//Add validation layers
const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };// Enable all validation layers available

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

class TriangleDisplayApplication
{
public:
    void run() 
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
private:

    GLFWwindow* window;
    VkInstance vkInstance; //connection between application and graphics driver. Provides driver with some details about the app.
    VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE; //Handle to physical graphics card
    VkDevice vkLogicalDevice;
    VkQueue vkQueue;


    void initWindow();
    bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
    void createInstance();
    void pickPhysicalDevice();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void createLogicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice vkPhysicalDevice);
    void initVulkan();
    void mainLoop();
    void cleanup();
};

int main() 
{
    TriangleDisplayApplication app;

    try 
    {
        app.run();
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void TriangleDisplayApplication::mainLoop()
{
    //Add event loop
    while (!glfwWindowShouldClose(window))
    {
        //Get user events such as key-press/ mouse-click
        glfwPollEvents();
    }
}

void TriangleDisplayApplication::cleanup()
{
    vkDestroyDevice(vkLogicalDevice, nullptr);
    vkDestroyInstance(vkInstance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void TriangleDisplayApplication::initWindow()
{
    glfwInit();

    //Tell GLFW to not create OpenGL context by default;
    //GLFW_CLIENT_API is used to specify contexts for OpenGL/OpenGL-ES
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //Handling resizing in Vulkan needs a special care, hence skipping for now
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //Parameters: 4th param: handle to monitor to open the window, 5th param: specific to OpenGL
    window = glfwCreateWindow(WIDTH, HEIGHT, "Triangle in Vulkan", nullptr, nullptr);

}

void TriangleDisplayApplication::initVulkan()
{
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
}

void TriangleDisplayApplication::createInstance()
{
    //Many structs in Vulkan have a pNext member that can point to extension information in the future
    VkApplicationInfo appInfo{}; //Optional but provides useful info to driver for optimization purposes
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Triangle in Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    //Since Vulkan is platform agnostic, we need an extension to interface it with windows system.
    //GLFW has a built-in function that returns the required extension(s)
    //Extensions are platform specific APIs that need to be invoked - OS/Drivers
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    //Add the RTX extensions here
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    //createInfo.enabledLayerCount = 0;

    if (enableValidationLayers && checkValidationLayerSupport(validationLayers))
    {
        createInfo.enabledLayerCount = 1;
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        throw std::runtime_error("validation layers requested, but not available!");
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &vkInstance);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

void TriangleDisplayApplication::pickPhysicalDevice()
{
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, &vkPhysicalDevice);

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            vkPhysicalDevice = device;
            break;
        }
    }

    if (vkPhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void TriangleDisplayApplication::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(vkPhysicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &vkLogicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
}

bool TriangleDisplayApplication::checkValidationLayerSupport(const std::vector<const char*>& validationLayers)
{
    //Fetch the available validation layers
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    //Enumerate the validation layers array into a vector
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }

    return true;
}

QueueFamilyIndices TriangleDisplayApplication::findQueueFamilies(VkPhysicalDevice device) {

    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            break;
        }

        i++;
    }

    return indices;
}

bool TriangleDisplayApplication::isDeviceSuitable(VkPhysicalDevice vkPhysicalDevice)
{
    /*VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &deviceFeatures);

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;*/

    QueueFamilyIndices indices = findQueueFamilies(vkPhysicalDevice);
    return indices.isComplete();
}
