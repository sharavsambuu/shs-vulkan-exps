add_executable (main main.cpp)

find_package (Vulkan REQUIRED FATAL_ERROR)
if (Vulkan_FOUND)
  include_directories  (main ${Vulkan_INCLUDE_DIRS})
  target_link_libraries(main ${Vulkan_LIBRARIES}   )
endif (Vulkan_FOUND)
