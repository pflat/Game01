
# Set CMake minimum version and CMake policy required by UrhoCommon module
cmake_minimum_required (VERSION 3.2.3)
if (COMMAND cmake_policy)
    # Libraries linked via full path no longer produce linker search paths
    cmake_policy (SET CMP0003 NEW)
    # INTERFACE_LINK_LIBRARIES defines the link interface
    cmake_policy (SET CMP0022 NEW)
    # Disallow use of the LOCATION target property - so we set to OLD as we still need it
    cmake_policy (SET CMP0026 OLD)
    # MACOSX_RPATH is enabled by default
    cmake_policy (SET CMP0042 NEW)
    # Honor the visibility properties for SHARED target types only
    cmake_policy (SET CMP0063 OLD)
endif ()

# Set project name
project (Game01)

# Set CMake modules search path
if (WIN32)
    set (CMAKE_MODULE_PATH ${URHO3D_HOME}/share/CMake/Modules)
elseif (UNIX)
    set (CMAKE_MODULE_PATH ${URHO3D_HOME}/share/Urho3D/CMake/Modules)
endif ()

# Include Urho3D Cmake common module
include (UrhoCommon)

# Find Urho3D library
find_package (Urho3D REQUIRED)

# DEPRECATED - pay attention to this macro
define_dependency_libs (Urho3D)

if (URHO3D_LIB_TYPE STREQUAL STATIC)
# This branch is untested
    add_library (Urho3DLib STATIC IMPORTED)
    set_target_properties (Urho3DLib PROPERTIES IMPORTED_LOCATION ${ABSOLUTE_PATH_LIBS})
elseif (URHO3D_LIB_TYPE STREQUAL SHARED)
    if (WIN32)
        add_library (Urho3DLib SHARED IMPORTED)
        set_target_properties (Urho3DLib PROPERTIES IMPORTED_LOCATION ${ABSOLUTE_PATH_LIBS})
    elseif (UNIX)
# This branch error needs to be solved
        message (FATAL_ERROR "I was unable to do the install process correctly using shared Urho3D library. Please build a static Urho3D library and build this project with that lib.")
    endif ()
endif ()


# Set project header files
#file (GLOB_RECURSE GAME01_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# Set project source files
file (GLOB_RECURSE GAME01_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

# Set header file search locations
#include_directories (include)
include_directories (${URHO3D_INCLUDE_DIRS})

# Set target binary of project
if (WIN32)
    add_executable (Game01 WIN32 ${GAME01_SOURCES})
elseif (UNIX)
    add_executable (Game01 ${GAME01_SOURCES})
endif ()


if (WIN32)
#    set_target_properties (Game01 PROPERTIES RUNTIME_OUTPUT_DIRECTORY win32/ )
#    target_link_libraries (Game01 ${ABSOLUTE_PATH_LIBS} ${LIBS} -mwindows)  --  VS 2010  ????
target_link_libraries (Game01 ${ABSOLUTE_PATH_LIBS} ${LIBS} -SUBSYSTEM:WINDOWS)
elseif (UNIX)
#    set_target_properties (Game01 PROPERTIES RUNTIME_OUTPUT_DIRECTORY linux/ )
    target_link_libraries (Game01 ${ABSOLUTE_PATH_LIBS} ${LIBS})
endif ()

# Install rules
################

# Set install rule for binary
install (TARGETS Game01 RUNTIME DESTINATION .)

# Set install rule for Urho3D.dll
if (${URHO3D_LIB_TYPE} STREQUAL SHARED AND WIN32)
    install (FILES ${URHO3D_DLL} DESTINATION .)
    if (WIN32)
        install (FILES ${DIRECT3D_DLL} DESTINATION .)
    endif ()
endif ()

# Set install rule for CoreData.pak
install (FILES CoreData.pak DESTINATION .)

# Set install rule for game01.settings
install (FILES game01.settings DESTINATION .)

# Set install rule for Data folder
install (DIRECTORY Data/ DESTINATION Data)
#FILES_MATCHING PATTERN "*.dae"  PATTERN "*.jpg")
