cmake_minimum_required(VERSION 3.0)
project(mc-pricer CXX)

# Prefix
if (NOT PREFIX)
    set(PREFIX ${CMAKE_CURRENT_BINARY_DIR})
endif (NOT PREFIX)
set(CMAKE_INSTALL_PREFIX ${PREFIX} CACHE STRING "Installation prefix." FORCE)

if (CXX)
    set(CMAKE_CXX_COMPILER ${CXX})
endif (CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Release or Debug
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif ()


# Detect Boost
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
message(STATUS "Boost include directory ${Boost_INCLUDE_DIR}")

# Detect PNL
find_package(Pnl REQUIRED)
set(LIBS ${LIBS} ${PNL_LIBRARIES})
include_directories(${PNL_INCLUDE_DIRS})
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-DPNL_RANGE_CHECK_OFF)
endif()

# Add math lib
set(LIBS ${LIBS} m)

# Add jlparser
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_subdirectory(3rdparty/jlparser)


file(GLOB SOURCES "../Win32Pricing/src/*.cpp")
file(GLOB HEADERS "../Win32Pricing/src/*.hpp")

file(GLOB SOURCESTEST "../Win32Pricing/src/*.cpp")
get_filename_component(full_path_pricer ${CMAKE_CURRENT_SOURCE_DIR}../Win32Pricing/src/pricer.cpp ABSOLUTE)
list (REMOVE_ITEM SOURCESTEST ${full_path_pricer})

file(GLOB HEADERSTEST "../Win32Pricing/src/*.hpp")


add_executable(pricer ../Win32Pricing/src/pricer.cpp ${SOURCES} ${HEADERS})
target_link_libraries(pricer jl_parser ${LIBS})

#file(GLOB HEADERSTEST "src/*.hpp")






# Print compiler flags
get_directory_property(DirDefs COMPILE_DEFINITIONS)
message(STATUS "COMPILE_DEFINITIONS = ${DirDefs}")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_LINKER_EXE_FLAGS}" )
else()
    message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_LINKER_EXE_FLAGS}" )
endif()

###############################################################
################## google test configuration ##################
###############################################################

enable_testing ()
# Download googletest
configure_file(gtest_config.txt googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

# Add googletest during compilation
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src)

include_directories(${gtest_SOURCE_DIR}/include)

###############################################################
########################### Tests #############################
###############################################################

#add_subdirectory(test)
#add_test (NAME AsianOptionTest COMMAND AsianOptionTest)
#add_test (NAME BasketOptionTest COMMAND BasketOptionTest)
#add_test (NAME PerformanceOptionTest COMMAND PerformanceOptionTest)
#add_test (NAME MonteCarloTest COMMAND MonteCarloTest)
#add_test (NAME BlackScholesTest COMMAND BlackScholesTest)



####################################################################
##############check#################################################
####################################################################
#add_custom_target(check make && ./test/BasketOptionTest)
                        #make && ./test/AsianOptionTest
                        #make && ./test/PerformanceOptionTest
                        #make && ./test/MonteCarloTest
                        #make && ./test/BlackScholesTest)

###############################################################
########################## Doxygen ############################
###############################################################

#add_custom_target(doc doxygen ../doc/doxy.rc)
