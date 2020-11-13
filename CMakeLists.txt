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
    src/main.cpp
    src/fs.cpp
    src/tree-model.cpp
	src/mio.hpp
	
	# GUI: Types of tree entry.
	# The only reason to make a new type is if
	# it needs to add more children to itself from data.
	src/tree-entries/tree-entry-sort-proxy.cpp
	src/tree-entries/tree-entry.cpp
	src/tree-entries/peg-entry.cpp
	src/tree-entries/vpp-entry.cpp
	
	src/format-readers/validation-error.cpp
	src/format-readers/vpp-v1.cpp
	src/format-readers/vpp-v2.cpp
	src/format-readers/vif-mesh.cpp
	src/format-readers/peg.cpp
    src/format-readers/peg-texture-decoder.cpp

	# Custom Qt widgets
	src/widgets/main-window.cpp
    src/widgets/file-info-frame.cpp

	# Qt widgets to view specific formats
    src/widgets/format-viewers/image-viewer.cpp
    src/widgets/format-viewers/plaintext-viewer.cpp
    src/widgets/format-viewers/p3d-viewer.cpp
	src/widgets/format-viewers/vim-viewer.cpp
    src/widgets/format-viewers/plaintext-viewer.cpp
)

set(ACE3X_FORMS
    ui/image-viewer.ui
    ui/plaintext-viewer.ui
    ui/p3d-viewer.ui
    ui/vim-viewer.ui
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
