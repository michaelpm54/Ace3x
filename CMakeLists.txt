cmake_minimum_required(VERSION 3.14)

project(Ace3x
	VERSION 0.1
	DESCRIPTION "GUI tool for viewing and exporting VPP2 archives."
	LANGUAGES CXX
)

set(ACE3X_MAIN_TARGET ace3x)

## DEPENDENCIES START ##

# Conan
include(cmake/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP CMAKE_TARGETS BUILD missing)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
find_package(Qt5 5.13 COMPONENTS Widgets REQUIRED)

## DEPENDENCIES END ##

## SOURCES START ##

set(ACE3X_SOURCES
    src/Main.cpp
    src/FrameDecoder.cpp
    src/Util.cpp
	
	# GUI: tree model
    src/tree-model.cpp
	
	# GUI: tree model entry
	src/tree-entry/tree-entry.cpp
	src/tree-entry/tree-entry-sort-proxy.cpp
	src/tree-entry/peg-entry.cpp
	src/tree-entry/vpp-entry.cpp
	
	# Summoner file formats
    src/Formats/VifMesh.cpp

	# Custom widgets
	src/Widgets/MainWindow.cpp
    src/Widgets/FileInspector.cpp
    src/Widgets/Viewers/Viewer.cpp
    src/Widgets/Viewers/ImageViewer.cpp
    src/Widgets/Viewers/PlaintextViewer.cpp
    src/Widgets/Viewers/P3DViewer.cpp
	src/Widgets/Viewers/VIMViewer.cpp

	# Widgets to view specific formats
	src/Widgets/Viewers/Viewer.cpp
    src/Widgets/Viewers/ImageViewer.cpp
    src/Widgets/Viewers/PlaintextViewer.cpp
    src/Widgets/Viewers/P3DViewer.cpp
    src/Widgets/Viewers/VIMViewer.cpp
)

set(ACE3X_FORMS
    ui/ImageViewer.ui
    ui/PlaintextViewer.ui
    ui/P3DViewer.ui
    ui/VIMViewer.ui
)

## SOURCES_END

## MAIN TARGET START ##

add_executable(${ACE3X_MAIN_TARGET}
    ${ACE3X_SOURCES}
    ${ACE3X_FORMS}
)

target_link_libraries(${ACE3X_MAIN_TARGET} PRIVATE
	Qt5::Widgets
	${CONAN_LIBS}
)

# Include src/ so we don't have to use relative includes
target_include_directories(${ACE3X_MAIN_TARGET} PUBLIC
	${CMAKE_SOURCE_DIR}/src
) 

# Enable warnings
target_compile_options(${ACE3X_MAIN_TARGET} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)

set_property(TARGET ${ACE3X_MAIN_TARGET} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${ACE3X_MAIN_TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)

## MAIN TARGET END ##

## POST INSTALL/AUXILIARY START ##

# Copy Qt DLL's to output
add_custom_command(
    TARGET ${ACE3X_MAIN_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::Core> $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE:Qt5::Widgets>
        $<TARGET_FILE_DIR:${ACE3X_MAIN_TARGET}>
)

install(TARGETS ${ACE3X_MAIN_TARGET} DESTINATION bin)

## POST INSTALL/AUXILIARY END ##
