cmake_minimum_required(VERSION 3.6.1)
project(Devstone)
# The version number.
set (Devstone_VERSION_MAJOR 0)
set (Devstone_VERSION_MINOR 0)

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -std=c++14")

## Dhrystone 
add_custom_command(OUTPUT dhry/dhry_1.o dhry/dhry_2.o
                   COMMAND make
                   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dhry
)

## CDBoost
add_executable(cdboost-devstone 
               src/cdboost-devstone.cpp
               src/cdboost-devstone-atomic.hpp
               dhry/dhry_1.o dhry/dhry_2.o
)
target_include_directories(cdboost-devstone 
                           PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cdboost/include
)
target_link_libraries(cdboost-devstone 
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

## Cadmium
add_executable(cadmium-devstone
               src/cadmium-devstone.cpp
)
target_include_directories(cadmium-devstone
                           PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)
target_link_libraries(cadmium-devstone
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

## A reference model used for developing and testing the model generator
add_executable(cadmium-ref-LI
               src/cadmium-ref-LI.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
               dhry/dhry_1.o dhry/dhry_2.o
               events.txt
)
target_include_directories(cadmium-ref-LI
                           PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)

