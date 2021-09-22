project(HelloLuaProject)

if(WIN32)
  find_package(Lua REQUIRED)
else()
  find_package(Lua 5.4 REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 20)

# Building raw Lua experiments
add_executable(HelloLua main.cpp)
target_include_directories(HelloLua PUBLIC ${LUA_INCLUDE_DIRS})
target_link_libraries(HelloLua ${LUA_LIBRARIES})
if(MSVC)
  file(COPY        ${CMAKE_CURRENT_SOURCE_DIR}/hello.lua
       DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
else()
  file(COPY        ${CMAKE_CURRENT_SOURCE_DIR}/hello.lua
       DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()


# Building with Sol2, which is bridge between C++ and Lua
add_executable(HelloSol hello_sol.cpp)
if(WIN32)
  target_include_directories(HelloSol PUBLIC ${LUA_INCLUDE_DIRS})
else()
  target_include_directories(HelloSol PUBLIC ${LUA_INCLUDE_DIR})
endif()
target_link_libraries(HelloSol 
  PRIVATE
  sol2::sol2 
  ${LUA_LIBRARIES}
  )

if(MSVC)
  file(COPY        ${CMAKE_CURRENT_SOURCE_DIR}/hello_sol.lua
       DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
else()
  file(COPY        ${CMAKE_CURRENT_SOURCE_DIR}/hello_sol.lua
       DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

