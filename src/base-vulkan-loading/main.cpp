#include <iostream>
#include <vector>
#include <set>
#include "SDL2/SDL.h" 
#include "SDL2/SDL_vulkan.h"
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
            SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN
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


    //


    std::cout<<"Одоогийн байдлаар бүгд хэвийн юм шиг байна ..."<<std::endl;



    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
