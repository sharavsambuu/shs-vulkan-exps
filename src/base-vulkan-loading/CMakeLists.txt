add_executable (main main.cpp)

find_package(SDL2   REQUIRED FATAL_ERROR)
find_package(Vulkan REQUIRED FATAL_ERROR)

include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})

target_link_libraries(main 
  PRIVATE ${SDL2_LIBRARIES}
  PUBLIC  ${Vulkan_LIBRARIES}
  )
