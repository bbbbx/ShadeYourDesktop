cmake_minimum_required( VERSION 3.1 )

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
if( "${CMAKE_GENERATOR}" MATCHES "(Visual Studio)" )
  set(CMAKE_CXX_FLAGS "/W4")
else()
  set(CMAKE_CXX_FLAGS "-Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

project( ShadeYourDesktop )

set( CMAKE_CXX_STANDARD 17 )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )

set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" FORCE )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw )

set( FFmpeg_INCLUDE "/opt/homebrew/include" CACHE STRING "" )
set( FFmpeg_LIB "/opt/homebrew/lib" CACHE STRING "" )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/ProgramOptions.hxx/include
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw/include
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${FFmpeg_INCLUDE}
)

if ( MSVC )
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

if ( APPLE )
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AppKit" )
endif()

set(
  SHADE_YOUR_DESKTOP_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Program.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Decoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/src/gl.c
)
if ( APPLE )
  list( APPEND SHADE_YOUR_DESKTOP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/put_window_behind_desktop_icons_darwin.mm" )
elseif( MSVC )
  list( APPEND SHADE_YOUR_DESKTOP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/put_window_behind_desktop_icons_win.cpp" )
endif()

add_executable( ShadeYourDesktop ${SHADE_YOUR_DESKTOP_SRC} )
target_link_directories( ShadeYourDesktop PRIVATE
  ${FFmpeg_LIB} )
target_link_libraries( ShadeYourDesktop
  glfw
  avformat
  avcodec
  swscale
  avutil
)

if ( MSCV )
  if ( ${CMAKE_VERSION} VERSION_LESS "3.6.0" )
    message( "\n\t[ WARNING ]\n\n\tCMake version is less then 3.6\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'ShadeYourDesktop' as StartUp Project in Visual Studio.\n")
  else()
    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PERPROTY VS_STARTUP_PROJECT ShadeYourDesktop)
  endif()
endif()
