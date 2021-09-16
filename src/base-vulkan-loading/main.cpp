#include <iostream>
#include <vector>
#include <set>
#ifdef _WIN32
    #include "SDL.h" 
    #include "SDL_vulkan.h"
    #include "glm/glm.hpp"
#elif __linux__
    #include "SDL2/SDL.h" 
    #include "SDL2/SDL_vulkan.h"
    #include "glm/glm.hpp"
#endif
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"


int main(int argc, char *argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow(
            "Hello Vulkan API with SDL2",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640,
            480,
            SDL_WINDOW_VULKAN
            );



    // Vulkan extension ачаалах, SDL2 цонхтой интерфэйс үүсгэхэд хэрэгтэй
    unsigned int ext_count = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &ext_count, nullptr);
    std::vector<const char*> ext_names(ext_count);
    SDL_Vulkan_GetInstanceExtensions(window, &ext_count, ext_names.data());
    std::cout<<"#### "<<ext_count<<" ширхэг vulkan extension байна :"<<std::endl;
    std::vector<std::string> found_extensions;
    for (unsigned int i=0; i<ext_count; i++) {
        std::cout<<i<<": "<<ext_names[i]<<std::endl;
        found_extensions.emplace_back(ext_names[i]);
    }
    found_extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    // Vulkan layer extension ачаалах
    std::vector<std::string> found_layers;
    unsigned int instance_layer_count = 0;
    vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
    std::vector<VkLayerProperties> instance_layer_names(instance_layer_count);
    vkEnumerateInstanceLayerProperties(&instance_layer_count, instance_layer_names.data());
    std::cout<<"#### "<<instance_layer_count<<" ширхэг instance layer байна :\n";
    std::vector<const char*> valid_instance_layer_names;
    std::set<std::string> lookup_layers;
    lookup_layers.emplace("VK_LAYER_NV_optimus");
    lookup_layers.emplace("VK_LAYER_KHRONOS_validation");
    int count(0);
    found_layers.clear();
    for (const auto& name : instance_layer_names)
    {
        std::cout<<count<<" : "<<name.layerName<<" : "<<name.description<<std::endl;
        auto it = lookup_layers.find(std::string(name.layerName));
        if (it != lookup_layers.end())
            found_layers.emplace_back(name.layerName);
        count++;
    }



    // Vulkan instance үүсгэх
    VkInstance vk_instance;

    std::vector<const char*> layer_names;
    for (const auto& layer : found_layers)
        layer_names.emplace_back(layer.c_str());
    std::vector<const char*> new_ext_names;
    for (const auto& ext : found_extensions)
        new_ext_names.emplace_back(ext.c_str());

    unsigned int api_version;
    vkEnumerateInstanceVersion(&api_version);
    std::cout<<"Vulkan instance API хувилбар : "<<api_version<<std::endl;

    // VkApplicationInfo бүтэц цэнэглэх
    VkApplicationInfo app_info  = {};
    app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext              = NULL;
    app_info.pApplicationName   = "Base Vulkan and SDL2 App";
    app_info.applicationVersion = 1;
    app_info.pEngineName        = "Base Engine";
    app_info.engineVersion      = 1;
    app_info.apiVersion         = VK_API_VERSION_1_0;

    // VkInstanceCreateInfo бүтэц цэнэглэх
    VkInstanceCreateInfo inst_info    = {};
    inst_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext                   = NULL;
    inst_info.flags                   = 0;
    inst_info.pApplicationInfo        = &app_info;
    inst_info.enabledExtensionCount   = static_cast<uint32_t>(new_ext_names.size());
    inst_info.ppEnabledExtensionNames = new_ext_names.data();
    inst_info.enabledLayerCount       = static_cast<uint32_t>(layer_names.size());
    inst_info.ppEnabledLayerNames     = layer_names.data();

    // Vulkan runtime instance үүсгэх
    std::cout<<"Vulkan instance үүсгэж байна..."<<std::endl;
    VkResult res = vkCreateInstance(&inst_info, NULL, &vk_instance);
    switch (res) {
        case VK_SUCCESS:
            std::cout<<"Vulkan instance амжилттай үүсгэгдлээ."<<std::endl;
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            std::cout<<"Vulkan дэмждэг драйвер олдсонгүй тул instance үүсгэлт амжилтгүй боллоо."<<std::endl;
            return -1;
        default:
            std::cout<<"Vulkan instance үүсгэхэд үл мэдэх алдаа гарлаа."<<std::endl;
            return -1;
    }



    // GPU төхөөрөмж сонгох
    VkPhysicalDevice gpu;

    // Боломжит төхөөрөмжүүдийн тоог авах, дор хаяж 1 ширхэг байх хэрэгтэй
    unsigned int physical_device_count(0);
    vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, nullptr);
    if (physical_device_count == 0) {
        std::cout<<"Хурдасгуур төхөөмрөмж олдсонгүй"<<std::endl;
        return -1;
    }
    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, physical_devices.data());

    // Төхөөрөмжийн мэдээллийг харуулах
    std::cout<<physical_device_count<<" ширхэг төхөөрөмж олдлоо : "<<std::endl;
    count = 0;
    std::vector<VkPhysicalDeviceProperties> physical_device_properties(physical_devices.size());
    for (auto& physical_device : physical_devices) {
        vkGetPhysicalDeviceProperties(physical_device, &(physical_device_properties[count]));
        std::cout<<count<<" : "<<physical_device_properties[count].deviceName<<std::endl;
        count++;
    }

    // Хэрэв олон төхөөрөмж байвал нэгийг нь сонгох
    unsigned int selection_id = 0;
    if (physical_device_count > 1) {
        while (true) {
            std::cout<<"Ашиглах төхөөрөмжийн дугаарыг оруулна уу : ";
            std::cin>>selection_id;
            if (selection_id>=physical_device_count || selection_id<0) {
                std::cout<<"Буруу утга байна, 0-ээс "<<physical_device_count-1<<"-ийн хооронд утга авна"<<std::endl;
                continue;
            }
            break;
        }
    }
    std::cout<<"сонголт : "<<physical_device_properties[selection_id].deviceName<<std::endl;
    VkPhysicalDevice selected_device = physical_devices[selection_id];

    // График командууд ажиллуулах чадвартай queue-тэй эсэхийг нягтлах
    unsigned int family_queue_count(0);
    vkGetPhysicalDeviceQueueFamilyProperties(selected_device, &family_queue_count, nullptr);
    if (family_queue_count==0) {
        std::cout<<"Энэ төхөөрөмж юм хийж чадахгүйнээ"<<std::endl;
        return -1;
    }

    // queue бүлийн шинжүүдийг авах
    std::vector<VkQueueFamilyProperties> queue_properties(family_queue_count);
    vkGetPhysicalDeviceQueueFamilyProperties(selected_device, &family_queue_count, queue_properties.data());

    // график комманд ажиллуулах чадвартай queue байна уу үгүй юу
    unsigned int queue_node_index = -1;
    for (unsigned int i=0; i<family_queue_count; i++) {
        if (queue_properties[i].queueCount>0 && queue_properties[i].queueFlags&VK_QUEUE_GRAPHICS_BIT) {
            queue_node_index = i;
            break;
        }
    }
    if (queue_node_index<0) {
        std::cout<<"Графиктай ажиллачихаар queue олдсонгүй ээ"<<std::endl;
        return -1;
    }



    // Физик төхөөрөмжтэй харилцан ажиллах логик төхөөрөмж
    VkDevice device;

    for (const auto& layer : found_layers)
        layer_names.emplace_back(layer.c_str());

    // График карт дээрхи ашиглах боломжтой extension-үүдийн тоог авах
    uint32_t device_property_count(0);
    if (vkEnumerateDeviceExtensionProperties(selected_device, NULL, &device_property_count, NULL)!=VK_SUCCESS) {
        std::cout<<"Төхөөрөмжийн extension-үүдийг тоог авч чадсангүй"<<std::endl;
        return -1;
    }
    std::cout<<"Энэ төхөөрөмж дээр нийт "<<device_property_count<<" extension олдлоо"<<std::endl;

    // Төхөөрөмж дээрхи extension-үүдийг нэрсийг авах
    std::vector<VkExtensionProperties> device_properties(device_property_count);
    if (vkEnumerateDeviceExtensionProperties(selected_device, NULL, &device_property_count, device_properties.data())!=VK_SUCCESS) {
        std::cout<<"Төхөөрөмжийн extension нэрийг авч чадсангүй"<<std::endl;
        return -1;
    }

    // Ашиглахаар хүссэн extension-үүдийн нэрийг тааруулах
    std::vector<const char*> device_property_names;
    std::set<std::string> required_extension_names;
    required_extension_names.emplace(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    count = 0;
    for (const auto& ext_property : device_properties) {
        std::cout<<count<<" : "<<ext_property.extensionName<<std::endl;
        auto it = required_extension_names.find(std::string(ext_property.extensionName));
        if (it != required_extension_names.end()) {
            device_property_names.emplace_back(ext_property.extensionName);
        }
        count++;
    }

    // Ашиглахаар хүссэн extension олдоогүй бол анхааруулах
    if (required_extension_names.size() != device_property_names.size()) {
        std::cout<<"Энэ төхөөрөмжинд ашиглачихаар extension одсонгүй!"<<std::endl;
        return -1;
    }

    std::cout<<std::endl;
    for (const auto& name : device_property_names)
        std::cout<<"Хэрэглэхээр тохируулсан extension: "<<name<<std::endl;

    // Физик төхөөрөмж дээр олдсон queue дээр график командууд боловсруулах үүрэгтэй queue үүсгэх
    VkDeviceQueueCreateInfo queue_create_info;
    queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_node_index;
    queue_create_info.queueCount       = 1;
    std::vector<float> queue_prio      = { 1.0f };
    queue_create_info.pQueuePriorities = queue_prio.data();
    queue_create_info.pNext            = NULL;
    queue_create_info.flags            = 0;

    // Төхөөрөмж үүсгэх мэдээлэл
    VkDeviceCreateInfo create_info;
    create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount    = 1;
    create_info.pQueueCreateInfos       = &queue_create_info;
    create_info.ppEnabledLayerNames     = layer_names.data();
    create_info.enabledLayerCount       = static_cast<uint32_t>(layer_names.size());
    create_info.ppEnabledExtensionNames = device_property_names.data();
    create_info.enabledExtensionCount   = static_cast<uint32_t>(device_property_names.size());
    create_info.pNext                   = NULL;
    create_info.pEnabledFeatures        = NULL;
    create_info.flags                   = 0;

    // Шинэ төхөөрөмж үүсгэхэд бэлэн боллоо
    res = vkCreateDevice(selected_device, &create_info, nullptr, &device);
    if (res!=VK_SUCCESS) {
        std::cout<<"Logic device үүсгэлт амжилтгүй боллоо!"<<std::endl;
        return -1;
    }



    // Рэндэр хийж хадгалах бүтэц
    VkSurfaceKHR presentation_surface;
    if (!SDL_Vulkan_CreateSurface(window, vk_instance, &presentation_surface)) {
        std::cout<<"SDL-ээр Vulkan-тай нийцтэй surface үүсгүүлж чадсангүй ээ"<<std::endl;
        return -1;
    }
    // Үүсгэсэн surface маань queue-ий төрөл болон GPU-тэй нийцтэй эсхийг нягтлах
    VkBool32 supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(selected_device, queue_node_index, presentation_surface, &supported);
    if (!supported) {
        std::cout<<"Surface-г төхөөрөмж дэмжихгүй байна!"<<std::endl;
        return -1;
    }
    std::cout<<"SDL нийцтэй Vulkan surface үүсгэгдлээ. Үүн дээр рэндэр хийгдэнэ."<<std::endl;



    // Рэндэрлэсэн үр дүнг арчуулчихалгүй зохицуулж байх зориулалттай Swap Chain бүтэц үүсгэх
    VkSwapchainKHR swap_chain = NULL;

    // swap-chain үүсгэхэд шаардлагатай surface-ийн гишүүн утгуудыг авах
    VkSurfaceCapabilitiesKHR surface_properties;
    if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(selected_device, presentation_surface, &surface_properties)!=VK_SUCCESS) {
        std::cout<<"Surface-ийн хийж чадах зүйлсийн утгуудыг авч чадсангүй"<<std::endl;
        return -1;
    }
    // Зураг харуулах горим (Синхрон биелэхээр, шууд биелэхээр гэх мэт)
    VkPresentModeKHR presentation_mode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
    uint32_t mode_count(0);
    if(vkGetPhysicalDeviceSurfacePresentModesKHR(selected_device, presentation_surface, &mode_count, NULL)!=VK_SUCCESS) {
        std::cout<<"Харуулах горимын утгуудыг авч чадсангүй!"<<std::endl;
        return -1;
    }
    std::vector<VkPresentModeKHR> available_modes(mode_count);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(selected_device, presentation_surface, &mode_count, available_modes.data())!=VK_SUCCESS) {
        std::cout<<"Харуулах горимын утгуудыг авч чадсангүй!"<<std::endl;
        return -1;
    }
    bool mode_found = false;
    for (auto& mode : available_modes) {
        if (mode==presentation_mode) {
            mode_found = true;
            break;
        }
    }
    if (!mode_found) {
        std::cout<<"Хүссэн харуулах горимоо авч чадсангүй, энгийн FIFO горимруу шилжлээ горимруу шилжлээ"<<std::endl;
        presentation_mode = VK_PRESENT_MODE_FIFO_KHR;
    }

    // swap chain-тай хамааралтай бусад утгуудыг авах
    unsigned int number           = surface_properties.minImageCount+1;
    unsigned int swap_image_count = number>surface_properties.maxImageCount?surface_properties.minImageCount:number;

    // Зурагны хэмжээг авах, заяамал утга нь цонхны хэмжээ
    VkExtent2D size = {(unsigned int)640, (unsigned int)480};
    // Цонхыг зурагн хэмжээнд нийцүүлийн сунгавал 
    if (surface_properties.currentExtent.width == 0xFFFFFFF) {
        size.width  = glm::clamp<unsigned int>(size.width , surface_properties.minImageExtent.width , surface_properties.maxImageExtent.width );
        size.height = glm::clamp<unsigned int>(size.height, surface_properties.minImageExtent.height, surface_properties.maxImageExtent.height);
    } else {
        size = surface_properties.currentExtent;
    }
    VkExtent2D swap_image_extent = size;

    // Зураг ашиглах зориулалтыг авах, өнгөнд зориулж уу, depth үү, stencil-д зориулах уу гэх мэт
    VkImageUsageFlags usage_flags;
    std::vector<VkImageUsageFlags> desired_usages;
    desired_usages.emplace_back(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    usage_flags = desired_usages[0];
    for (const auto& desired_usage : desired_usages) {
        VkImageUsageFlags image_usage = desired_usage & surface_properties.supportedUsageFlags;
        if (image_usage != desired_usage) {
            std::cout<<desired_usage<<" image usage-д дэмжилт байхгүй"<<std::endl;
            return -1;
        }
        usage_flags = (usage_flags|desired_usage);
    }
    VkSurfaceTransformFlagBitsKHR transform;
    if (surface_properties.supportedTransforms&VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        std::cout<<"Дэмжилт хийгдээгүй surface transform : "<<VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        transform = surface_properties.currentTransform;
    }

    // swapchain-ы зургийн форматыг авах 
    VkSurfaceFormatKHR image_format;
    unsigned int format_count(0);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(selected_device, presentation_surface, &format_count, nullptr)!=VK_SUCCESS) {
        std::cout<<"surface-ийн форматын мэдээллийг авч чадсангүй"<<std::endl;
        return -1;
    }
    std::vector<VkSurfaceFormatKHR> found_formats(format_count);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(selected_device, presentation_surface, &format_count, found_formats.data())!=VK_SUCCESS) {
        std::cout<<"surface-ийн форматын мэдээллийг авч чадсангүй"<<std::endl;
        return -1;
    }
    // Энэ тохиолдолд ямар нэгэн хязгаарлалт байхгүй, хүссэнээ хэрэглэж болно
    if (found_formats.size()==1 && found_formats[0].format==VK_FORMAT_UNDEFINED) {
        image_format.format     = VK_FORMAT_B8G8R8A8_SRGB;
        image_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }
    bool is_format_found = false;
    // Бусад тохиолдолд хоёулаа дэмжилттэй эсхийг шалгах
    for (const auto& found_format_outer : found_formats) {
        if (found_format_outer.format == VK_FORMAT_B8G8R8A8_SRGB) {
            image_format.format = found_format_outer.format;
            for (const auto& found_format_inner : found_formats) {
                // Color space олдлоо
                if (found_format_inner.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    image_format.colorSpace = found_format_inner.colorSpace;
                    is_format_found = true;
                    break;
                }
            }
            // Таарсан color space алгах, эхнийхийг нь сонгох
            std::cout<<"warning: таарах color space алгах, эхний боломжтойг нь сонголоо!"<<std::endl;
            image_format.colorSpace = found_formats[0].colorSpace;
            is_format_found = true;
            break;
        }
    }
    if (!is_format_found) {
        std::cout<<"warning: таарах color space алгах, эхний боломжтойг нь сонголоо!"<<std::endl;
        image_format = found_formats[0];
    }
    // Хуучин swap chain
    VkSwapchainKHR old_swap_chain = swap_chain;
    // swapchain үүсгэх мэдээллийг цуглуулах
    VkSwapchainCreateInfoKHR swap_info;
    swap_info.pNext                 = nullptr;
    swap_info.flags                 = 0;
    swap_info.surface               = presentation_surface;
    swap_info.minImageCount         = swap_image_count;
    swap_info.imageFormat           = image_format.format;
    swap_info.imageColorSpace       = image_format.colorSpace;
    swap_info.imageExtent           = swap_image_extent;
    swap_info.imageArrayLayers      = 1;
    swap_info.imageUsage            = usage_flags;
    swap_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    swap_info.queueFamilyIndexCount = 0;
    swap_info.pQueueFamilyIndices   = nullptr;
    swap_info.preTransform          = transform;
    swap_info.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swap_info.presentMode           = presentation_mode;
    swap_info.clipped               = true;
    swap_info.oldSwapchain          = NULL;
    swap_info.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    // Хуучин swap chain-ыг устгах
    if (old_swap_chain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, old_swap_chain, nullptr);
        old_swap_chain = VK_NULL_HANDLE;
    }
    // Шинийг үүсгэх
    if (vkCreateSwapchainKHR(device, &swap_info, nullptr, &old_swap_chain)!=VK_SUCCESS) {
        std::cout<<"swap chain үүсгэж чадсангүй"<<std::endl;
        return -1;
    }

    swap_chain = old_swap_chain;


    // swap chain-аас зураг хариуцсан объектийг авах
    std::vector<VkImage> chain_images;
    unsigned int image_count(0);
    res = vkGetSwapchainImagesKHR(device, swap_chain, &image_count, nullptr);
    if (res!=VK_SUCCESS) {
        std::cout<<"swap chain дэх зурагнуудын тоог авч чадсангүй"<<std::endl;
        return -1;
    }
    chain_images.clear();
    chain_images.resize(image_count);
    if (vkGetSwapchainImagesKHR(device, swap_chain, &image_count, chain_images.data())!=VK_SUCCESS) {
        std::cout<<"swap chain-аас зураг хариуцсан объект авч чадсангүй"<<std::endl;
        return -1;
    }


    // Рэндэрлэх командууд илгээх зориулалттай queue-г авах
    VkQueue graphics_queue;
    vkGetDeviceQueue(device, queue_node_index, 0, &graphics_queue);



    std::cout<<"Хурдасгуур төхөөрөмж(GPU) болон Vulkan-г амжилттай тохируулж үүсгэлээ."<<std::endl;
    std::cout<<"Цонх болон түүнд таарах vulkan surface-г амжилттай үүсгэлээ."<<std::endl;
    std::cout<<"Swapchain-г амжилттай үүсгэлээ"<<std::endl;
    std::cout<<"Рэндэрлэх командууд илгээхэд бэлэн!"<<std::endl;



    // Энэ хэсэг юм рэндэрлэх кодууд хийгдэнэ
    bool run = true;
    while (run)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                run = false;
            }
        }
    }


    // Ашигласан нөөцүүдээ суллах
    vkDestroySwapchainKHR(device, swap_chain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(vk_instance, presentation_surface, nullptr);
    vkDestroyInstance(vk_instance, nullptr);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
