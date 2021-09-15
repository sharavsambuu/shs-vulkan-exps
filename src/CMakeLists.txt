add_executable (main main.cpp)

find_package (Vulkan REQUIRED FATAL_ERROR)

#target_include_directories (main ${Vulkan_INCLUDE_DIR})
#target_link_libraries (main LINK_PUBLIC ${Vulkan_LIBRARY})