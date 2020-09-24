



cmake_minimum_required(VERSION 3.14)

project(SKYLINE)

set(WINSTATICLIBS "-static" "-ljpeg" "-ldumb" "-lwebp" "-lFLAC" "-ltheora" "-lvorbisfile" "-lvorbis" "-logg" "-lphysfs" "-lfreetype" "-lpng16" "-lzlibstatic" "-ldsound" "-lgdiplus" "-luuid" "-lkernel32" "-lwinmm" "-lpsapi" "-lopengl32" "-lglu32" "-luser32" "-lcomdlg32" "-lgdi32" "-lshell32" "-lole32" "-ladvapi32" "-lws2_32" "-lshlwapi" "-lpthread" "-static-libstdc++" "-static-libgcc")


file(GLOB_RECURSE OBJSRCS "src/*.cpp")

add_executable(SKYLINE ${OBJSRCS})

target_include_directories(SKYLINE PUBLIC "/usr/include" "/usr/local/include")
target_link_directories(SKYLINE PUBLIC "/usr/lib" "/usr/local/lib")

target_compile_options(SKYLINE PUBLIC "-Wall" "-Wextra" "-Wshadow")
target_compile_definitions(SKYLINE PUBLIC "-DALLEGRO_STATICLINK")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   target_compile_options(SKYLINE PUBLIC "-g")
   target_compile_definitions(SKYLINE PUBLIC "-DEAGLE_ASSERT_EXCEPTIONS" "-DEAGLE_DEBUG" "-DDEBUG" "-DEAGLE_LOG")
endif()

if (WIN32)
   if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_link_libraries(SKYLINE PUBLIC "-lEagleA5Debug" "-lEagleDebug" "-lallegro_monolith-debug-static")
   else()
      target_link_libraries(SKYLINE PUBLIC "-lEagleA5" "-lEagle" "-lallegro_monolith-static")
   endif()

   target_link_libraries(SKYLINE PUBLIC ${WINSTATICLIBS})
endif()

if (UNIX)
   if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_link_libraries(SKYLINE PUBLIC "-lEagleA5DebugShared" "-lEagleDebugShared" "-lallegro_monolith-debug" "-lphysfs" "-lOpenGL" "-lGLU")
   else()
      target_link_libraries(SKYLINE PUBLIC "-lEagleA5Shared" "-lEagleShared" "-lallegro_monolith" "-lphysfs" "-lOpenGL" "-lGLU")
   endif()
endif()

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
   target_link_options(SKYLINE PUBLIC "-s")
endif()

install(TARGETS SKYLINE DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY "Data" DESTINATION ${CMAKE_INSTALL_PREFIX})
