# Set the program name
set(PROGRAM_NAME C-bind)
project(${PROGRAM_NAME} C)

# Requirements
cmake_minimum_required(VERSION 3.5)


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
set (CMAKE_C_FLAGS "-O0 -g -Wall -Wextra -Werror")

# Name things
set(BIND bind.so)
set(MAIN main.out)


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


# Shared Libraries
add_library(${BIND} SHARED bind.c bind_vector.c bind_blank.c bind_utilities.c )

# Executables
add_executable(${MAIN} main.c )

# Link shared librares
target_link_libraries(${MAIN} ${BIND})
