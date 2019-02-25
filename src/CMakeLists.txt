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
#                                               #
#                    Set by user                #
#                                               #
#################################################


# For YouCompleteMe vim plugin
# Disable for non-developers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


#################################################
#                                               #
#                Program Constants              #
#                                               #
#################################################


# Bind library constants
set(BIND bind.so)

# Example case constants
set(CMAKE_C_FLAGS "-Wall -Wextra")
set(TESTNV test-non-systemv.out)
set(TESTV test.out)


#################################################
#                                               #
#                 Bind library                  #
#                                               #
#################################################


# Bind library
add_library(${BIND} SHARED bind.c bind_vec.c bind_utilities.c )
set_target_properties(${BIND} PROPERTIES COMPILE_FLAGS "-O0 -Wall -Wextra -Werror")
target_link_libraries(${BIND} Threads::Threads)


#################################################
#                                               #
#                 Example cases                 #
#                                               #
#################################################


# Example cases
add_executable(${TESTV} test.c )
add_executable(${TESTNV} test-non-systemv.c )
target_link_libraries(${TESTV} ${BIND})
target_link_libraries(${TESTNV} ${BIND})
