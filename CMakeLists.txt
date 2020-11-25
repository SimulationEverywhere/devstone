cmake_minimum_required(VERSION 3.6.1)
project(Devstone)
# The version number.
set (Devstone_VERSION_MAJOR 0)
set (Devstone_VERSION_MINOR 0)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

include(CheckCXXCompilerFlag)

# Check for standard to use
check_cxx_compiler_flag(-std=c++17 HAVE_FLAG_STD_CXX17)
if(HAVE_FLAG_STD_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic --std=c++17 -fPIC -g -fopenmp")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic --std=c++1z -fopenmp")
endif()

#find_package(Boost COMPONENTS program_options unit_test_framework REQUIRED)
find_package(Boost 1.56.0 COMPONENTS program_options unit_test_framework system thread REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

include_directories(include ${Boost_INCLUDE_DIRS})

# Dhrystone
# add_custom_command(OUTPUT dhry/dhry_1.o dhry/dhry_2.o
#                    COMMAND make
#                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dhry
# )

## CDBoost
add_executable(cdboost-devstone
               src/cdboost-devstone.cpp
               src/cdboost-devstone-atomic.hpp
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
               events.txt
)
target_include_directories(cadmium-dynamic-devstone
                           PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)
target_link_libraries(cadmium-dynamic-devstone
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

## Cadmium dynamic, concurrent version
add_executable(cadmium-dynamic-conc-devstone
               src/cadmium-dynamic-devstone.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
               events.txt
)
#target_compile_definitions(cadmium-dynamic-conc-devstone
#                           PUBLIC CADMIUM_EXECUTE_CONCURRENT
#)

#target_link_libraries(cadmium-dynamic-conc-devstone
#        ${Boost_PROGRAM_OPTIONS_LIBRARY}
#)

target_compile_definitions(cadmium-dynamic-conc-devstone PUBLIC CADMIUM_EXECUTE_CONCURRENT BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION BOOST_THREAD_PROVIDES_EXECUTORS BOOST_THREAD_USES_MOVE)


target_include_directories(cadmium-dynamic-conc-devstone
        PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)
target_link_libraries(cadmium-dynamic-conc-devstone PUBLIC ${Boost_LIBRARIES})

#target_link_libraries(cadmium-dynamic-conc-devstone
#        ${Boost_PROGRAM_OPTIONS_LIBRARY}
#)


## Cadmium dynamic, openMP parallel version
add_executable(cadmium-dynamic-cpu-parallel-devstone
               src/cadmium-dynamic-devstone.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
               events.txt
)
target_compile_definitions(cadmium-dynamic-cpu-parallel-devstone
                           PUBLIC CPU_PARALLEL
)
target_include_directories(cadmium-dynamic-cpu-parallel-devstone
        PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)

target_link_libraries(cadmium-dynamic-cpu-parallel-devstone
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
)


## Cadmium dynamic, openMP only lambda parallel version
add_executable(cadmium-dynamic-cpu-lambda-parallel-devstone
               src/cadmium-dynamic-devstone.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
               events.txt
)
target_compile_definitions(cadmium-dynamic-cpu-lambda-parallel-devstone
                           PUBLIC CPU_LAMBDA_PARALLEL
)
target_include_directories(cadmium-dynamic-cpu-lambda-parallel-devstone
        PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)

target_link_libraries(cadmium-dynamic-cpu-lambda-parallel-devstone
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
)


## Cadmium dynamic, openMP only delta parallel version
add_executable(cadmium-dynamic-cpu-delta-parallel-devstone
               src/cadmium-dynamic-devstone.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
               events.txt
)
target_compile_definitions(cadmium-dynamic-cpu-delta-parallel-devstone
                           PUBLIC CPU_DELTA_PARALLEL
)
target_include_directories(cadmium-dynamic-cpu-delta-parallel-devstone
        PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)

target_link_libraries(cadmium-dynamic-cpu-delta-parallel-devstone
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
)



## Reference models used for developing and testing the model generators
add_executable(cadmium-ref-LI
               src/cadmium-ref-LI.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
               events.txt
)
target_include_directories(cadmium-ref-LI
                           PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
)

add_executable(cadmium-ref-HI
               src/cadmium-ref-HI.cpp
               src/cadmium-devstone-atomic.hpp src/cadmium-event-reader.hpp
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
                       test/test_helpers.hpp)
        target_include_directories(${testName}
                                   PUBLIC ${PROJECT_SOURCE_DIR}/simulators/cadmium/include
        )
        target_link_libraries(${testName} PUBLIC ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
	      add_test(${testName} ${testName})
endforeach(testSrc)

##LI is 3x3
# TODO: This test should be rewritten comparing both models using DEVDiagrammer Json instead of source
add_test(NAME Cadmium_generator_LI_3x3
         COMMAND test/check_generated_LI_against_ref
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
