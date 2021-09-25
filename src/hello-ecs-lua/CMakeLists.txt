project(HelloLuaProject)

if(WIN32)
  find_package(Lua REQUIRED)
else()
  find_package(Lua 5.4 REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 20)


# Basic Flecs ECS experiments
add_executable(HelloFlecs hello_flecs.cpp)
if(WIN32)
  target_include_directories(HelloFlecs PUBLIC ${LUA_INCLUDE_DIRS})
else()
  target_include_directories(HelloFlecs PUBLIC ${LUA_INCLUDE_DIR})
endif()
target_link_libraries(HelloFlecs
  PRIVATE
  sol2::sol2 
  flecs_static
  ${LUA_LIBRARIES}
  )


# Flecs and Sol3 integration exps
add_executable(HelloFlecsSol hello_flecs_sol.cpp)
if(WIN32)
  target_include_directories(HelloFlecsSol PUBLIC ${LUA_INCLUDE_DIRS})
else()
  target_include_directories(HelloFlecsSol PUBLIC ${LUA_INCLUDE_DIR})
endif()
target_link_libraries(HelloFlecsSol
  PRIVATE
  sol2::sol2 
  flecs_static
  ${LUA_LIBRARIES}
  )

