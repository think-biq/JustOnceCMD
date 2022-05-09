CMAKE_MINIMUM_REQUIRED (VERSION 3.2)

# Setup versioning.
set(BUILD_MAJOR 1)
set(BUILD_MINOR 1)
set(BUILD_PATCH 3)
set(BUILD_VERSION "${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH}")
# Expose version information to config.
set(VERSION_NAME_BUFFER_SIZE 16)
set(VERSION_MAJOR ${BUILD_MAJOR})
set(VERISON_MINOR ${BUILD_MINOR})
set(VERSION_PATCH ${BUILD_PATCH})
set(VERSION_FORMAT "v%d.%d.%d")

project (JustOnceCMD VERSION ${BUILD_VERSION})

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE PATH "" FORCE)
endif()

string(
    COMPARE EQUAL
    "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
    IS_STANDALON_PROJECT
)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(FLAT_INSTALL "Set library and runtime directory to output" OFF)

if(FLAT_INSTALL)
# Make sure libraries will be places beside executable.
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

add_subdirectory(lib/JustOnce)
add_subdirectory(lib/argparse)
add_subdirectory(lib/libqrencode)

file (GLOB JustOnceCMD_SRC
	"${PROJECT_SOURCE_DIR}/src/*.c")
add_executable(JustOnceCMD ${JustOnceCMD_SRC})
configure_file (in/config.h.in 
	${CMAKE_INSTALL_INCLUDEDIR}/config.h @ONLY)
target_include_directories(JustOnceCMD
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/JustOnce/src>"
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/argparse/>"
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/libqrencode/>"
  PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include"
)
target_link_libraries(JustOnceCMD JustOnce argparse qrencode)
set_property(TARGET JustOnceCMD PROPERTY C_STANDARD 11)

if(WIN32)
   #VS_DEBUGGER_COMMAND_ARGUMENTS()
   set_target_properties(JustOnceCMD PROPERTIES 
       VS_DEBUGGER_COMMAND ""
       VS_DEBUGGER_COMMAND_ARGUMENTS "-U -a \"neo@matr.ix\" -i \"architect\" -l 23 -d 7 -k \"$(ProjectDir)..\\Key.hash\"")
endif()

#[[install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include"
    FILES_MATCHING PATTERN "*.h"
)

install(
    TARGETS JustOnceCMD
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)]]