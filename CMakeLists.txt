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

include(FindCurl)
find_package(SDL2 CONFIG REQUIRED)
find_package(jansson CONFIG REQUIRED)
find_package(unofficial-minizip CONFIG REQUIRED)

if(WIN32)
	find_package(PThreads4W)

	if(NOT PThreads4W_FOUND)
		find_package(Threads REQUIRED)
	endif()

	add_executable(disasterlauncher WIN32 "appicon.rc")
else()
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)

	add_executable(disasterlauncher "")
endif()

target_sources(disasterlauncher PRIVATE "main.c" "gui.c" "web.c" "log.c" "components.c" "label.c" "fs.c" "tasks.c" "image.c" "task_launch.c" "task_install.c" "task_dotnet.c" "sock.c")

if(WIN32)
	target_link_libraries(disasterlauncher PRIVATE SDL2::SDL2main SDL2::SDL2 jansson::jansson unofficial::minizip::minizip ${CURL_LIBRARIES} wsock32 ws2_32)
else()
	target_link_libraries(disasterlauncher PRIVATE SDL2::SDL2main SDL2::SDL2 jansson::jansson unofficial::minizip::minizip ${CURL_LIBRARIES})
endif()

add_compile_definitions("DEBUG=$<CONFIG:Debug>")
message("Compiler flags: ${CMAKE_CXX_FLAGS_RELEASE}")

install(
  TARGETS disasterlauncher
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})