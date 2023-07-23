cmake_minimum_required (VERSION 3.8)
project(disasterlauncher)

if(MSVC)
	cmake_policy(SET CMP0091 NEW)

	if (POLICY CMP0141)
	  cmake_policy(SET CMP0141 NEW)
	  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
	endif()

	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if(MSVC)
	include(FindCurl)
	find_package(SDL2 CONFIG REQUIRED)
	find_package(jansson CONFIG REQUIRED)
	find_package(unofficial-minizip CONFIG REQUIRED)
	find_package(PThreads4W)

	if(NOT PThreads4W_FOUND)
		find_package(Threads REQUIRED)
	endif()

	add_executable(disasterlauncher WIN32 "appicon.rc")
	target_link_libraries(disasterlauncher PRIVATE SDL2::SDL2main SDL2::SDL2 jansson::jansson unofficial::minizip::minizip ${CURL_LIBRARIES} wsock32 ws2_32)
else()
	include(FindPkgConfig)

	if(NOT PKG_CONFIG_FOUND)
		message(FATAL_ERROR "Failed to find pkg_config!")
	endif()

	find_package(SDL2 CONFIG REQUIRED)
	pkg_check_modules(CURL libcurl)
	pkg_check_modules(JSON jansson)
	pkg_check_modules(MINIZ minizip)

	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)

	add_executable(disasterlauncher "")
	target_link_libraries(disasterlauncher PRIVATE SDL2::SDL2main SDL2::SDL2 ${JSON_LIBRARIES} ${MINIZ_LIBRARIES} ${CURL_LIBRARIES})
endif()

target_sources(disasterlauncher PRIVATE "main.c" "gui.c" "web.c" "log.c" "components.c" "label.c" "fs.c" "tasks.c" "image.c" "task_launch.c" "task_install.c" "sock.c")

# add_compile_definitions("DEBUG=$<CONFIG:Debug>")
# message("Compiler flags: ${CMAKE_CXX_FLAGS_RELEASE}")

install(
  TARGETS disasterlauncher
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})