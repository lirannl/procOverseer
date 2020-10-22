cmake_minimum_required(VERSION 2.6.0)

# here we specify that the project is C language only, so the default
# C compiler on the system will be used
project(procOverseer C)

#Bring the headers, such as Student.h into the project
include_directories(controller)
include_directories(overseer)

#Can manually add the sources using the set command as follows:
set(CONTROLLER controller/controller.c)
set(SERVER overseer/overseer.c)

add_executable(controller ${CONTROLLER})
add_executable(server ${SERVER})