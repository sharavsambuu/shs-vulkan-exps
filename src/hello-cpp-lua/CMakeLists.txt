project(HelloLuaProject)

find_package(Lua 5.4 REQUIRED)

include_directories(${LUA_INCLUDE_DIRS})

add_executable(HelloLua main.cpp)

target_link_libraries(HelloLua
  ${LUA_LIBRARIES}
  )

file(COPY        ${CMAKE_CURRENT_SOURCE_DIR}/hello.lua
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})