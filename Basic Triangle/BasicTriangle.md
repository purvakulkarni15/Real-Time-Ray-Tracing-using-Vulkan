# Aim: To render a simple triangle using Vulkan

## Windows Setup:

1. Download LunarSDK( includes the headers and a loader for the Vulkan functions to look up the functions in the driver at runtime, similar to GLEW for OpenGL) for Vulkan.Link: https://vulkan.lunarg.com/sdk/home#windows
2. Install GLFW  for creating a window to display the rendered results. Link: https://www.glfw.org/
3. Install GLM for vector algebra operations. Link:https://github.com/g-truc/glm/releases
4. Add Include/ and lib/ paths as additional includes and libraries is VS Studio
5. Set the language compiler to C++17

## How to draw a triangle?


## 1. Setup
### Steps:
1. Create window and event handling loop using GLFW.
2. Create a vulkan instance (VkInstance) by querying necessary instance-level extensions and enabling the validation layer.
3. Pick a supporting physical graphics device (VkPhysicalDevice) from the available list. Extensions such as "device support to RT" should be checked at this step.
4. Create a logical device (VkDevice) to select the features from physical device and submit them through queues (VkQueue)
### Key Learnings:

![Components flow chart](https://github.com/purvakulkarni15/Real-Time-Ray-Tracing-using-Vulkan/blob/main/Basic%20Triangle/Flowcharts/vulkan1.png)

#### Standard steps:
##### Create/Destroy an object
1. Fill a struct/s with information necessary for object creation.
   Struct struc = {}; This means either zero initialize or initialize every member.
3. Call vkCreate---(--,--,--, &object) and pass object as the out parameter.
4. Use vkDestroy---() to deallocate its memory.
##### Get list of available options
1. vKEnumerate----(--, &count, nullptr)
2. Create a DS to hold options: vector<VKType> vec(count)
3. vKEnumerate----(--,&count, vec.data())
   
#### 1. Extensions:
  1. A graphics card may have a unique functionality that is exposed to developers by Vulkan via extensions.
  2. Developer needs to query any such extensions/capabilities explicitly while creating VkInstance.
  3. Eg. NVIDIA's hardware support to Ray Tracing Pipeline can be added via VK_KHR_ray_tracing_pipeline and other extensions.
#### 2. Validation layers:
  1. Vulkan driver is designed to have minimal overhead and hence the error checking is left entirely to the user.
  2. Validation layers are functionality provided by Vulkan to additionally enable checking such nullptrs, resource allocation, thread safety, etc.
  3. Enabling a validation layer simply means calling a proxy function that does the error-checking before proceeding to the main function call.
#### 3. Queue families:
  1. Every operation in Vulkan,(drawing, uploading textures, memory transfer) requires commands to be submitted to a queue. 
  2. There are different types of queues coming from different queue families and each family of queues allows only a subset of commands.
  3. For example, a queue family that only allows memory transfer related commands.
#### 4. Physical and Logical Devices:
  1. Physical Device (VKPhysicalDevice) gives us all the properties of a device. (Read only)
  2. Logical Device (VKDevice) is used to interact with the driver of the physical device chosen through VkQueues. (Repr of physical device. Vulkan's window into Graphics card)

## 2. Presentation
### Steps:
1. Create a connection of vulkan with windows system to present results via WSI extensions.(Done by GLFW internally)
2. Create a swap chain object to hold the image objects used for rendering by vulkan.
3. Create ImageView container to contain swap chain images. To view any image (VkImage) we need to access it via VkImageView.

### Key Learnings
   
![Presentation flow chart](https://github.com/purvakulkarni15/Real-Time-Ray-Tracing-using-Vulkan/blob/main/Basic%20Triangle/Flowcharts/vulkan2.png)

### 1. WSI Extensions
1. VK_KHR_surface exposes a VkSurfaceKHR object that represents an abstract type of surface to present rendered images. The surface in our program will be backed by the window created using GLFW. 
2. VK_KHR_SURFACE: This extension is included in list given by glfwGetRequiredInstanceExtensions. (Instance level extension)

### 2. Swap Chains
   1. Vulkan does not have default frame buffer to render and needs a structure called swap chain to store these buffers.
   2. The swap chain is a queue of images that are waiting to be presented to the screen. 
   3. The general purpose of the swap chain is to synchronize the presentation of images with the refresh rate of the screen.
   4. There are three kinds of properties we need to check to create a swap chain
   - Basic surface capabilities (min/max number of images in swap chain, min/max width and height of images)
   - Surface formats (pixel format, color space)
   - Available presentation modes(conditions for "swapping" images to the screen.
   5. VK_KHR_Swapchains: Device level extension
   6. Link to tutorial: https://www.youtube.com/watch?v=nSzQcyQTtRY
   
### 3. Presentation modes
   1. VK_PRESENT_MODE_IMMEDIATE_KHR: Images produced by application are transferred directly to screen which may result in tearing.
   2. VK_PRESENT_MODE_FIFO_KHR: Display takes an image from the front of the swap chain queue when the display is refreshed. Program inserts rendered images at the
   back of the queue. `vertical blank: The moment the display is refreshed.`
   3. VK_PRESENT_MODE_FIFO_RELAXED_KHR: If the application is late and the queue was empty at the last vertical blank, the image is transferred directlyand may result
   in visible tearing.
   4. VK_PRESENT_MODE_MAILBOX_KHR: Variation of the second mode. When the queue is full, the images that are already queued are replaced with the newer ones. 

### 4. Swap extent
1. The swap extent is the resolution of the swap chain images.
2. The range of the possible resolutions is defined in the VkSurfaceCapabilitiesKHR structure. 
3. The resolution of the window needs to be matched by setting the width and height in the currentExtent member. 
5. GLFW uses two units when measuring sizes: pixels and screen coordinates. 

### 5. Image Tearing
#### Problem
1. A graphics adapter has a pointer to a surface that represents the image being displayed on the monitor, called a front buffer. 
2. When monitor is refreshed, the graphics card sends the contents of the front buffer to the monitor to be displayed. 
3. The monitor's refresh rates are very slow in comparison to the rest of the computer. (60 Hz (60 times per second) to 100 Hz.)
4. If your application is updating the front buffer while the monitor is in the middle of a refresh, the image that is displayed will be cut in half with the upper half of the display containing the old image and the lower half containing the new image. 
5. This problem is referred to as tearing.
#### Solution - Back buffering 
1. vertical retrace (or vertical sync) operation: Monitor refreshes its image by moving a light pin horizontally, from the top left of the monitor and ending at the bottom right. When the light pin reaches the bottom, the monitor recalibrates the light pin by moving it back to the upper left so that the process can start again. This recalibration is called a vertical sync. 
2. During a vertical sync, the monitor is not drawing anything, so any update to the front buffer will not be seen until the monitor starts to draw again. The vertical sync is relatively slow; however, not slow enough to render a complex scene while waiting. What is needed to avoid tearing and be able to render complex scenes is a process called back buffering.

### 6. Image View
   1. Image has two parts - memory(data) and View. 
   2. Image view is a protocol used to access the image in vulkan.
   
## 3. Graphics Pipeline
### 1. Simplified overview:
1. Input assembler: collects the raw vertex/indices data from the specified buffers.
2. Vertex shader: Runs for every vertex and generally applies transformations to turn vertex positions from model space to screen space. It also passes per-vertex data down the pipeline.
3. Tessellation shader: Allows to subdivide geometry based on certain rules to increase the mesh quality.
4. Geometry shader: Runs on every primitive (triangle, line, point) and can discard it or output more primitives than came in. (Not used much in today's applications because the performance is not that good on most graphics cards except for Intel's integrated GPUs.)
5. Rasterization: Discretizes the primitives into fragments-pixel elements that they fill on the framebuffer. Any fragments that fall outside the screen are discarded and the attributes outputted by the vertex shader are interpolated across the fragments. Usually the fragments that are behind other primitive fragments are also discarded here because of depth testing.
6. Fragment shader: Invoked for every fragment that survives rasterization. Determines the color using the interpolated data from the vertex shader, which can include things like texture coordinates and normals for lighting.
7. Color blending: Applies operations to mix different fragments that map to the same pixel in the framebuffer. Fragments can simply overwrite each other, add up or be mixed based upon transparency.
   
### Steps
1. Write shaders and compile them using glslc to get bytecodes in .spv format.
2. Load the shader binaries and create a shader module.
3. Create pipeline stage objects each for every shader module   
   
### Key Learnings
#### 1. SPIR-V: 
   1. Unlike OpenGL, shader code in vulkan has to be specified in bytecode format called as SPIR-V.
   2. Advantage: Compilers written by GPU vendors to turn shader code into native code are significantly less complex. With GLSL, some GPU vendors used their interpretation of the standard which might cause some vendor's drivers rejecting your code due to syntax errors. With a straightforward bytecode format like SPIR-V that will hopefully be avoided.
#### glslc: a command line compiler for GLSL/HLSL to SPIR-V
#### 2. Dynamic state: `VKPipelineDynamicStateCreateInfo`
   1. Most of the states of the pipeline needs to be baked into it at the time of its creation.
   2. The exceptions are: size of the viewport, blend constants and line size.
   3. We need to state this via VKPipelineDynamicStateCreateInfo struct.
#### 3. Vertex Input: `VKPipelineVertexInputStageCreateInfo`
   1. Defines format of the vertex data
   2. Bindings: spacing between data and whether the data is per-vertex or per-instance.
   3. Attribute descriptions: type of the attributes passed to the vertex shader, which binding to load them from and at which offset.
#### 4. Input Assembly: `VkPipelineInputAssemblyStateCreateInfo`
   1. Describes two things: Topology, primitive restart enable
   2. Topology:
      1. VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
      2. VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every 2 vertices without reuse
      3. VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: the end vertex of every line is used as start vertex for the next line
      4. VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: triangle from every 3 vertices without reuse
      5. VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: the second and third vertex of every triangle are used as first two vertices of the next triangle
   2. The vertices are loaded from the vertex buffer by index in sequential order.
   3. Element buffer specifies the indices to use per primitive. 
#### 5. Rasterization:
   1. Geometry -> Fragments
   2. Performs depth and face culling
#### 6. Render pass:
   1. Tells Vulkan about the framebuffer attachments that will be used while rendering. 
   2. We need to specify no. of color and depth buffers, no. of samples to use how their contents should be handled throughout the rendering operations. 
   3. This information is wrapped in a render pass object.
   4. loadOp: what to do with the data in the attachment before rendering
      VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
      VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
      VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them
   5. storeOp: what to do with the data in the attachment after rendering
      VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
      VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the rendering operation

#### 7. Image Layouts
   1. Images in the GPU aren???t necessarily in the format you would expect. 
   2. For optimization purposes, the GPUs perform a lot of transformation and reshuffling of them into internal opaque formats. 
   For example, some GPUs will compress textures whenever they can, and will reorder the way the pixels are arranged so that they mipmap better. 
   3. In Vulkan, there is control over the layout for the image, which lets the driver transform the image to those optimized internal formats.
   VK_IMAGE_LAYOUT_UNDEFINED : Don???t care of what the layout is, can be whatever.
   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : Image is on a layout optimal to be written into by rendering commands.
   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : Image is on a layout that allows displaying the image on the screen.
   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : (Used later) Image is on a format optimized to be read from shaders.

## 4. Drawing:
### Steps:
   1. Wait for the previous frame to finish
   2. Acquire an image from the swap chain
   3. Record a command buffer which draws the scene onto that image
   4. Submit the recorded command buffer
   5. Present the swap chain image
### Key Learnings:
#### 1. Framebuffers
   1. Framebuffers are images that are used to render to in the graphics pipeline stage.
   2. These images are later submitted to swap chain via present queue to be presented on our window.
   3. Hence, the format of the framebuffers should be similar to the one's in the swapchain.
#### 2. Command Pool and Command Buffers
   1. Command pools manage memory. Command buffers are allocated from them.
   2. We pass all the rendering commands to command buffers all at once.
   3. The command buffer is submitted to the graphics queue to the the device driver.
   4. The driver then rearranges the commands based on its optimization strategies.
#### 3. Synchronization:
   1. In Vulkan, synchronization of execution on the GPU is explicit. 
   2. The order of operations has to be defined using various synchronization primitives.
   3. Many vulkan API calls that start executing work on the GPU are asynchronous, the functions will return before the operation has finished.
   4. Example: Each of these events is set in motion using a single function call, but are all executed asynchronously.
      1. Acquire an image from the swap chain
      2. Execute commands that draw onto the acquired image
      3. Present that image to the screen for presentation, returning it to the swapchain
   But each of the operations depends on the previous one finishing. Thus we need to explore which primitives we can use to achieve the desired ordering.
   ##### Semaphores
   1. A semaphore is used to add order between queue operations. 
   2. There are two types - binary and timeline.
   3. It is either unsignaled or signaled. 
   4. It begins life as unsignaled. 
   5. Example:
      1. Assume we have semaphore S and queue operations A and B that we want to execute in order. 
      2. We tell Vulkan that operation A will 'signal' semaphore S when it finishes executing, and operation B will 'wait' on semaphore S before it begins executing.
      3. When operation A finishes, semaphore S will be signaled, while operation B wont start until S is signaled. 
      4. After operation B begins executing, semaphore S is automatically reset back to being unsignaled, allowing it to be used again.

   ##### Fences
   1. Used for ordering the execution on the CPU (host). To let host know when the GPU has finished something, we use a fence.
   2. Example: Taking a screenshot
      1. Assume we have already done the necessary work on the GPU. 
      2. We need to transfer the image from the GPU over to the host and then save the memory to a file. 
      3. We have command buffer A which executes the transfer and fence F. 
      4. We submit command buffer A with fence F, then immediately tell the host to wait for F to signal. 
      5. This causes the host to block until command buffer A finishes execution. 
      6. Thus we are safe to let the host save the file to disk, as the memory transfer has completed.

   
