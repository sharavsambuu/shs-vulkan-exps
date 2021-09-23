project(HelloLuaProject)

if(WIN32)
  find_package(Lua REQUIRED)
else()
  find_package(Lua 5.4 REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 20)


add_executable(HelloECS main.cpp)
if(WIN32)
  target_include_directories(HelloECS PUBLIC ${LUA_INCLUDE_DIRS})
else()
  target_include_directories(HelloECS PUBLIC ${LUA_INCLUDE_DIR})
endif()
target_link_libraries(HelloECS
  PRIVATE
  sol2::sol2 
  ${LUA_LIBRARIES}
  EnTT::EnTT
  )


