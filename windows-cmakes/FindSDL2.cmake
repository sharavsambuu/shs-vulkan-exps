set(SDL2_DIR $ENV{SDL2_PATH})

message(STATUS "setting up SDL2 Path: $ENV{SDL2_PATH}")


set(SDL2_INCLUDE_DIRS "$ENV{SDL2_PATH}/include")

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
  set(SDL2_LIBRARIES "$ENV{SDL2_PATH}/lib/x64/SDL2.lib;$ENV{SDL2_PATH}/lib/x64/SDL2main.lib")
else ()
  set(SDL2_LIBRARIES "$ENV{SDL2_PATH}/lib/x86/SDL2.lib;$ENV{SDL2_PATH}/lib/x86/SDL2main.lib")
endif ()

string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)