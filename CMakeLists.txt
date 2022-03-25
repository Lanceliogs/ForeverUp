project(ForeverUp LANGUAGES CXX)

cmake_minimum_required(VERSION 3.5.0)

set(CMAKE_AUTOMOC ON)  # Va chercher les moc_files dans le dossier build
set(CMAKE_AUTOUIC ON)  # Va chercher les .ui files
set(CMAKE_AUTORCC ON)  # Va chercher les .qrc files
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Compilation flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-attributes -Wno-unknown-pragmas -std=gnu++0x")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Widgets REQUIRED) # On a besoin de Qt5

# GIT SNAP
add_custom_target(GIT_SNAP
    COMMAND call git_snap.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

# GUI
file(GLOB_RECURSE GUI_SRC_LIST src/*)
add_executable(ForeverUp WIN32 ${GUI_SRC_LIST})
target_link_libraries (ForeverUp ${Qt5Widgets_LIBRARIES}) # Linking

# Uncomment this to add the version inside the executable
add_dependencies(ForeverUp GIT_SNAP)
