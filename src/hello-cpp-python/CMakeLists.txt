project(HelloCppPython)

 set(CMAKE_CXX_STANDARD 20)

# Basic Python embedding exp
add_executable(HelloPython hello_python.cpp)
target_link_libraries(HelloPython
  PRIVATE
  pybind11::embed
  )
if(MSVC)
  file(COPY        ${CMAKE_CURRENT_SOURCE_DIR}/hello.py
       DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
else()
  file(COPY        ${CMAKE_CURRENT_SOURCE_DIR}/hello.py
       DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()