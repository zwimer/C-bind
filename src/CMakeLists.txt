# Set the program name
set(PROGRAM_NAME C-bind)
project(${PROGRAM_NAME} C)

# Requirements
cmake_minimum_required(VERSION 3.5)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
if (NOT ${CMAKE_USE_PTHREADS_INIT})
	message(FATAL_ERROR "*** Pthreads not found ***")
endif()

#################################################
#												#
#					Set by user 				#
#												#
#################################################


# Debug mode
# Comment out to turn off
# set(DEBUG_MODE "On")

# For YouCompleteMe vim plugin
# Disable for non-developers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


#################################################
#												#
#				Program Constants				#
#												#
#################################################


# Required complication flags
# set (CMAKE_C_FLAGS "-O0 -g -Wall -Wextra")
set (CMAKE_C_FLAGS "-O0 -g -Wall -Wextra")

# Name things
set(BIND bind.so)
set(TESTV test.out)
set(TESTNV test-non-systemv.out)


#################################################
#												#
#				Program Constants				#
#												#
#################################################


# Debug mode
if(DEFINED DEBUG_MODE)
    message("Debug Mode ON!")
	add_definitions(-DDEBUG_MODE)
endif()

# Bind library
add_library(${BIND} SHARED bind.c bind_vec.c bind_utilities.c )
target_link_libraries(${BIND} Threads::Threads)

# Executables
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-int-conversion -Wno-pointer-to-int-cast")
add_executable(${TESTV} test.c )
add_executable(${TESTNV} test-non-systemv.c )
target_link_libraries(${TESTV} ${BIND})
target_link_libraries(${TESTNV} ${BIND})
