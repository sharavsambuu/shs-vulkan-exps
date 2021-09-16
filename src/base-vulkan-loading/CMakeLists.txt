project(BaseProject)

find_package(SDL2    REQUIRED)
find_package(Vulkan  REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS}  )
include_directories(${Vulkan_INCLUDE_DIRS})

add_executable(BaseProject main.cpp)

target_link_libraries(BaseProject 
  ${SDL2_LIBRARIES}
  ${Vulkan_LIBRARIES}
  )
