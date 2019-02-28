cmake_minimum_required(VERSION 3.6.1)
project(Devstone)
# The version number.
set (Devstone_VERSION_MAJOR 0)
set (Devstone_VERSION_MINOR 0)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      OFF)

include(CheckCXXCompilerFlag)

# Check for standard to use
check_cxx_compiler_flag(-std=c++17 HAVE_FLAG_STD_CXX17)
if(HAVE_FLAG_STD_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic --std=c++17 -fPIC")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic --std=c++1z")
endif()

find_package(Boost COMPONENTS program_options system thread REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

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
                      ${Boost_LIBRARIES}
)

## Reference models used for developing and testing the model generators
add_executable(LI_3_3
               src/LI_3_3.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
               dhry/dhry_1.o dhry/dhry_2.o
               events.txt
)
target_include_directories(LI_3_3
                           PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)
target_link_libraries(LI_3_3
                      ${Boost_LIBRARIES}
)

## Reference models used for developing and testing the model generators
add_executable(cadmium-ref-LI
               src/cadmium-ref-LI.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
               dhry/dhry_1.o dhry/dhry_2.o
               events.txt
)
target_include_directories(cadmium-ref-LI
                           PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)
target_link_libraries(cadmium-ref-LI
                      ${Boost_LIBRARIES}
)

add_executable(cadmium-ref-HI
               src/cadmium-ref-HI.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
               dhry/dhry_1.o dhry/dhry_2.o
               events.txt
)
target_include_directories(cadmium-ref-HI
                           PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)
target_link_libraries(cadmium-ref-HI
                      ${Boost_LIBRARIES}
)

## Test that generated matches ref
enable_testing()

##LI is 3x3
add_test(NAME Cadmium_generator_LI_3x3
         COMMAND ../test/check_generated_LI_against_ref.sh
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
