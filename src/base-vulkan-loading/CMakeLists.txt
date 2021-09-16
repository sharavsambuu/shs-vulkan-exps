project(BaseProject)

find_package(SDL2    REQUIRED)
find_package(Vulkan  REQUIRED)
find_package(glm     REQUIRED)

include_directories(${GLM_INCLUDE_DIRS}   )
include_directories(${SDL2_INCLUDE_DIRS}  )
include_directories(${Vulkan_INCLUDE_DIRS})

add_executable(BaseProject main.cpp)

target_link_libraries(BaseProject 
  ${SDL2_LIBRARIES}
  ${Vulkan_LIBRARIES}
  )
