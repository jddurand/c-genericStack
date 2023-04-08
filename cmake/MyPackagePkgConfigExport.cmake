#
# Based on https://stackoverflow.com/questions/44292462/how-to-auto-generate-pkgconfig-files-from-cmake-targets
#
function(auto_pc TARGET)
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-PKGCONFIG-DEBUG] Running on target ${TARGET}")
  ENDIF ()

  string(JOIN "," PUBLIC_DEPENDENCIES ${${TARGET}_public_dependencies})

  file(CONFIGURE OUTPUT "pc.${TARGET}/CMakeLists.txt"
       CONTENT [[
cmake_minimum_required(VERSION 3.16)
project(pc_@TARGET@)

message(STATUS "pc_@TARGET@: Starting")

message(STATUS "pc_@TARGET@: Requiring config of @TARGET@")
find_package(@TARGET@ REQUIRED CONFIG)

message(STATUS "pc_@TARGET@: Generating ${CMAKE_CURRENT_BINARY_DIR}/@TARGET@.pc")
file(GENERATE OUTPUT @TARGET@.pc
     CONTENT [=[
Name: @TARGET@
Requires: @PUBLIC_DEPENDENCIES@
Cflags: $<IF:$<BOOL:$<TARGET_PROPERTY:INTERFACE_INCLUDE_DIRECTORIES>>,-I$<JOIN:$<TARGET_PROPERTY:INTERFACE_INCLUDE_DIRECTORIES>, -I>,> $<IF:$<BOOL:$<TARGET_PROPERTY:INTERFACE_COMPILE_DEFINITIONS>>,-D$<JOIN:$<TARGET_PROPERTY:INTERFACE_COMPILE_DEFINITIONS>, -D>,>
Cflags.private: $<IF:$<BOOL:$<TARGET_PROPERTY:INTERFACE_INCLUDE_DIRECTORIES>>,-I$<JOIN:$<TARGET_PROPERTY:INTERFACE_INCLUDE_DIRECTORIES>, -I>,> $<IF:$<BOOL:$<TARGET_PROPERTY:INTERFACE_COMPILE_DEFINITIONS>>,-D$<JOIN:$<TARGET_PROPERTY:INTERFACE_COMPILE_DEFINITIONS>, -D>,> -D@TARGET@_STATIC
Libs: $<TARGET_LINKER_FILE_DIR:@TARGET@::@TARGET@>/$<TARGET_LINKER_FILE_NAME:@TARGET@::@TARGET@>
Libs.private: $<TARGET_LINKER_FILE_DIR:@TARGET@::@TARGET@_static>/$<TARGET_LINKER_FILE_NAME:@TARGET@::@TARGET@_static>
]=]   TARGET "@TARGET@::@TARGET@")
]] @ONLY NEWLINE_STYLE LF)

  file(CONFIGURE OUTPUT "pc.${TARGET}/post-install.cmake"
       CONTENT [[
#
# Recuperate all the cmake files for this target
#
file(REAL_PATH "${CMAKE_INSTALL_PREFIX}" cmake_install_prefix)
message(STATUS "@TARGET@: cmake_install_prefix is ${cmake_install_prefix}")
file(REAL_PATH "@CMAKE_BINARY_DIR@" cmake_binary_dir) # Top-level binary dir
message(STATUS "@TARGET@: cmake_binary_dir is ${cmake_binary_dir}")
file(REAL_PATH "@CMAKE_CURRENT_BINARY_DIR@" cmake_current_binary_dir)
message(STATUS "@TARGET@: cmake_current_binary_dir is ${cmake_current_binary_dir}")

set(proj "${cmake_current_binary_dir}/pc.@TARGET@")
execute_process(COMMAND "@CMAKE_COMMAND@" "-DCMAKE_PREFIX_PATH=${cmake_install_prefix}" -S "${proj}" -B "${proj}/build")
file(COPY "${proj}/build/@TARGET@.pc" DESTINATION "${cmake_install_prefix}")
]] @ONLY NEWLINE_STYLE LF)

  GET_PROPERTY(remaining_post_installs GLOBAL PROPERTY MYPACKAGE_REMAINING_POST_INSTALLS)
  IF (NOT DEFINED remaining_post_installs)
    set (remaining_post_installs 1)
  ELSE ()
    math(EXPR remaining_post_installs "${remaining_post_installs} + 1")
  ENDIF ()
  message(STATUS "Number of post-installs: ${remaining_post_installs}")
  SET_PROPERTY(GLOBAL PROPERTY MYPACKAGE_REMAINING_POST_INSTALLS ${remaining_post_installs})
  #
  # We initialize the post-install counter only on the top-level project
  # We intentionaly overwrite CMAKE_BINARY_DIR and not CMAKE_CURRENT_BINARY_DIR
  #
  SET (REMAINING_POST_INSTALLS_PATH ${CMAKE_BINARY_DIR}/remaining_post_installs.txt)
  IF (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    INSTALL (CODE "FILE(WRITE ${REMAINING_POST_INSTALLS_PATH} \"${remaining_post_installs}\")")
  ENDIF ()
  #
  SET (FIRE_POST_INSTALL_CMAKE_PATH ${CMAKE_BINARY_DIR}/fire_post_installs.cmake)
  IF (remaining_post_installs EQUAL 1)
    #
	# First time: initialize fire_post_installs.cmake
	#
    FILE(WRITE ${FIRE_POST_INSTALL_CMAKE_PATH} [[]])
  ENDIF ()
  FILE(APPEND ${FIRE_POST_INSTALL_CMAKE_PATH} "include(${CMAKE_CURRENT_BINARY_DIR}/pc.${TARGET}/post-install.cmake)\n")
  #
  # At each install we decrement the number of remaining post installs, and fire all of them when the number is 0
  #
  INSTALL(CODE "
  file(READ \"${REMAINING_POST_INSTALLS_PATH}\" remaining_post_installs)
  math(EXPR remaining_post_installs \"\${remaining_post_installs} - 1\")
  FILE(WRITE \"${REMAINING_POST_INSTALLS_PATH}\" \"\${remaining_post_installs}\")
  message(STATUS \"Remaining post-installs: \${remaining_post_installs}\")
  if (remaining_post_installs LESS_EQUAL 0)
    message(STATUS \"Firing post-installs\")
    execute_process(COMMAND \"${CMAKE_COMMAND}\" -P \"${FIRE_POST_INSTALL_CMAKE_PATH}\")
  endif ()
  ")
  # INSTALL (SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/pc.${TARGET}/post-install.cmake")
endfunction()

MACRO (MYPACKAGEPKGCONFIGEXPORT)
  auto_pc(${PROJECT_NAME})
  # Clean up install path
  install(CODE [[ file(REMOVE_RECURSE "${CMAKE_INSTALL_PREFIX}/_auto_pc") ]])
ENDMACRO ()
