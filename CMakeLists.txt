project(RLAIHex)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
include_directories(/home/alejandro/Code/svn-ramirezs/ARS/build/lib)


set(CMAKE_BUILD_TYPE Release) #Set this to Release or Debug depending on the type of executable desired.

#Information to add all the boost libraries.
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.53.0.0 COMPONENTS serialization) #Look for the boost package. The version can be obtained via: dpkg -s libboost-dev | grep 'Version'

#We will generate the executable iff the corresponding libraries have been found.
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME} ${SRC_LIST})
    #ARS Namespace libraries
    target_link_libraries(${PROJECT_NAME} /home/alejandro/Code/svn-ramirezs/ARS/build/lib/libclogger.a)
    target_link_libraries(${PROJECT_NAME} /home/alejandro/Code/svn-ramirezs/ARS/build/lib/libcmath.a)
    #Boost libraries.
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} pthread)
endif()
