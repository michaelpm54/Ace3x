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

	src/vfs/mio.hpp
	src/vfs/vfs.hpp
	src/vfs/vfs-entry.hpp
	src/vfs/mmap-vfs.hpp
	src/vfs/mmap-vfs.cpp
	
	src/tree-model/tree-model.hpp
	src/tree-model/tree-model.cpp
	src/tree-model/sort-proxy.hpp
	src/tree-model/sort-proxy.cpp
	
	src/formats/vf2.hpp

	src/format-readers/vim.hpp
	src/format-readers/vim.cpp
	src/format-readers/vpp.hpp
	src/format-readers/vpp.cpp
	src/format-readers/peg.hpp
	src/format-readers/peg.cpp
	src/format-readers/vf2.hpp
	src/format-readers/vf2.cpp
	src/format-readers/peg-texture-decoder.hpp
    src/format-readers/peg-texture-decoder.cpp
	src/format-readers/validation-error.hpp
	src/format-readers/validation-error.cpp
	src/format-readers/archive-entry.hpp

	# Custom Qt widgets
	src/widgets/main-window.cpp
    src/widgets/file-info-frame.cpp
	src/widgets/view-manager.cpp

	# Qt widgets to view specific formats
	src/widgets/format-viewers/viewer.cpp
    src/widgets/format-viewers/image-viewer.cpp
    src/widgets/format-viewers/plaintext-viewer.cpp
    src/widgets/format-viewers/p3d-viewer.cpp
	src/widgets/format-viewers/vim-viewer.cpp
    src/widgets/format-viewers/plaintext-viewer.cpp
	src/widgets/format-viewers/vf2-viewer.cpp
	src/widgets/format-viewers/empty-viewer.cpp
)

set(ACE3X_FORMS
	ui/main-window.ui
	ui/file-info-frame.ui
    ui/image-viewer.ui
    ui/p3d-viewer.ui
    ui/plaintext-viewer.ui
    ui/vim-viewer.ui
	ui/vf2-viewer.ui
	ui/empty-viewer.ui
)

## SOURCES_END

## MAIN TARGET START ##

add_executable(${ACE3X_MAIN_TARGET}
    ${ACE3X_SOURCES}
    ${ACE3X_FORMS}
	resources/resources.qrc
)

target_compile_definitions(${ACE3X_MAIN_TARGET} PRIVATE
	_CRT_SECURE_NO_WARNINGS
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
  $<$<CXX_COMPILER_ID:MSVC>:/W3>
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
