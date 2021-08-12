CMAKE_MINIMUM_REQUIRED (VERSION 3.2)

project (JustOnceCMD)

string(
    COMPARE EQUAL
    "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
    IS_STANDALON_PROJECT
)

add_subdirectory(third-party/JustOnce)
add_subdirectory(third-party/argparse)

file (GLOB JustOnceCMD_SRC
	"${PROJECT_SOURCE_DIR}/src/main.c")
add_executable(JustOnceCMD ${JustOnceCMD_SRC})
target_include_directories(JustOnceCMD PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third-party/JustOnce/src>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third-party/argparse/>"
)
target_link_libraries(JustOnceCMD JustOnce argparse)

#[[if (JustOnce_WithTest)
	if (IS_STANDALON_PROJECT)
		add_subdirectory(third-party/testly)
		include_directories(third-party/testly/src)
	else()
		find_package(Testly)
	endif()

	file(GLOB JustOnce_Test_SRC	
		"${PROJECT_SOURCE_DIR}/test/*.c"
		"${PROJECT_SOURCE_DIR}/test/**/*.c")
	add_executable (JustOnceTest ${JustOnce_Test_SRC})
	target_include_directories(JustOnce PUBLIC
	  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
	)
	target_link_libraries(JustOnceTest JustOnce Testly)
endif()]]