cmake_minimum_required(VERSION 3.26.0 FATAL_ERROR)
project(genericStack VERSION 1.1.17 LANGUAGES C)
#
# Get library helper
#
include(FetchContent)
FetchContent_Declare(cmake-helpers
  GIT_REPOSITORY https://github.com/jddurand/cmake-helpers.git
  GIT_SHALLOW TRUE
  GIT_REMOTE_UPDATE_STRATEGY CHECKOUT # We always want the latest version
)
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
