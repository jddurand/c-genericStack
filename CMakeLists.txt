#
# Multi-config generator, e.g. Visual Studio on Windows:
#
# cmake -S c-genericStack -B c-genericStack-build
# cmake --build c-genericStack-build --config RelWithDebInfo
# ctest --test-dir c-genericStack-build -C RelWithDebInfo
# Windows:
#   cmake --install c-genericStack-build --config RelWithDebInfo --prefix %cd%/c-genericStack-install
# Others:
#   cmake --install c-genericStack-build --config RelWithDebInfo --prefix `pwd`/c-genericStack-install
# cmake --build c-genericStack-build --config RelWithDebInfo --target package
#
# Single-config generator, e.g. NMake Makefiles on Windows, Unix Makefiles on Linxu:
#
# cmake -S c-genericStack -B c-genericStack-build -DCMAKE_BUILD_TYPE=RelWithDebInfo
# cmake --build c-genericStack-build
# ctest --test-dir c-genericStack-build
# Windows:
#   cmake --install c-genericStack-build --prefix %cd%/c-genericStack-install
# Others:
#   cmake --install c-genericStack-build --prefix `pwd`/c-genericStack-install
# cmake --build c-genericStack-build --target package
#
cmake_minimum_required(VERSION 3.26.0 FATAL_ERROR)
project(genericStack VERSION 1.1.23 LANGUAGES C)
#
# Get library helper
#
include(FetchContent)
if("x$ENV{CMAKE_HELPERS_DEPEND_CMAKE_HELPERS_FILE}" STREQUAL "x")
  FetchContent_Declare(cmake-helpers GIT_REPOSITORY https://github.com/jddurand/cmake-helpers.git GIT_SHALLOW TRUE)
else()
  FetchContent_Declare(cmake-helpers URL $ENV{CMAKE_HELPERS_DEPEND_CMAKE_HELPERS_FILE})
endif()
FetchContent_MakeAvailable(cmake-helpers)
#
# Create library
#
cmake_helpers_library(genericStack)
#
# Create test executables
#
include(CTest)
configure_file(test/config.h.in include/test/config.h)
cmake_helpers_exe(genericStackTester                          TEST TRUE SOURCES test/genericStack.c)
cmake_helpers_exe(genericStackTesterDefaultInitialStack       TEST TRUE SOURCES test/genericStack_defaultinitialstack.c)
cmake_helpers_exe(genericStackTesterSmallInitialStack         TEST TRUE SOURCES test/genericStack_smallinitialstack.c)
cmake_helpers_exe(genericStackTesterNoInitialStack            TEST TRUE SOURCES test/genericStack_noinitialstack.c)
cmake_helpers_exe(genericStackTesterMemset                    TEST TRUE SOURCES test/genericStackMemset.c)
cmake_helpers_exe(genericStackTesterDefaultInitialStackMemset TEST TRUE SOURCES test/genericStack_defaultinitialstackMemset.c)
cmake_helpers_exe(genericStackTesterSmallInitialStackMemset   TEST TRUE SOURCES test/genericStack_smallinitialstackMemset.c)
cmake_helpers_exe(genericStackTesterNoInitialStackMemset      TEST TRUE SOURCES test/genericStack_noinitialstackMemset.c)
#
# Package
#
cmake_helpers_package()
