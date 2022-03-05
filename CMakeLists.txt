cmake_minimum_required( VERSION 3.1 )

project( ShadeYourDesktop )

set( CMAKE_CXX_STANDARD 14 )


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw/include
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only" FORCE )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" FORCE )

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw )

if ( MSVC )
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

if ( APPLE )
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AppKit" )
endif()

set(
  SHADE_YOUR_DESKTOP_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/set_window_behind_desktop_icon.mm
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/src/gl.c
)

add_executable( ShadeYourDesktop ${SHADE_YOUR_DESKTOP_SRC} )
target_link_libraries( ShadeYourDesktop glfw )

file(
  COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
  DESTINATION ${CMAKE_BINARY_DIR}
)

# add_executable( video_decoder src/video_decoder.c )
# target_link_directories( video_decoder PRIVATE ./lib )
# target_link_libraries( video_decoder avformat avcodec )

if ( MSCV )
  if ( ${CMAKE_VERSION} VERSION_LESS "3.6.0" )
    message( "\n\t[ WARNING ]\n\n\tCMake version is less then 3.6\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'ShadeYourDesktop' as StartUp Project in Visual Studio.\n")
  else()
    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PERPROTY VS_STARTUP_PROJECT ShadeYourDesktop)
  endif()
endif()