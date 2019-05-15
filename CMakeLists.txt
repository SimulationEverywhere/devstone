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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic --std=c++17 -fPIC -g")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic --std=c++1z")
endif()

find_package(Boost COMPONENTS program_options unit_test_framework REQUIRED)

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
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

## Reference models used for developing and testing the model generators
add_executable(cadmium-dynamic-devstone
               src/cadmium-dynamic-devstone.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
               dhry/dhry_1.o dhry/dhry_2.o
               events.txt
)
target_include_directories(cadmium-dynamic-devstone
                           PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)
target_link_libraries(cadmium-dynamic-devstone
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
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

add_executable(cadmium-ref-HI
               src/cadmium-ref-HI.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
               dhry/dhry_1.o dhry/dhry_2.o
               events.txt
)
target_include_directories(cadmium-ref-HI
                           PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)

## Test that generated matches ref
enable_testing()
# Unit tests
FILE(GLOB TestSources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/*_test.cpp)
foreach(testSrc ${TestSources})
        get_filename_component(testName ${testSrc} NAME_WE)
        add_executable(${testName}
                       test/main-test.cpp
                       ${testSrc}
                       dhry/dhry_1.o dhry/dhry_2.o test/test_helpers.hpp)
        target_include_directories(${testName}
                                   PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
        )
        target_link_libraries(${testName} PUBLIC ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
	      add_test(${testName} ${testName})
endforeach(testSrc)

##LI is 3x3
# TODO: This test should be rewritten comparing both models using DEVDiagrammer Json instead of source
add_test(NAME Cadmium_generator_LI_3x3
         COMMAND test/check_generated_LI_against_ref.sh
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
