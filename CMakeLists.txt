cmake_minimum_required(VERSION 3.14)

project(Ace3x
	VERSION 0.1
	DESCRIPTION "GUI tool for viewing and exporting VPP2 archives."
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-padded")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

set(EXE_NAME ace3x)

# Qt stuff
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
list(APPEND ACE3X_LIBS Qt5::Widgets)
list(APPEND ACE3X_INCLUDES ${Qt5Widgets_INCLUDE_DIRS})

find_package(ZLIB REQUIRED)
list(APPEND ACE3X_LIBS ZLIB::ZLIB)
list(APPEND ACE3X_INCLUDES ${ZLIB_INCLUDE_DIR})

set(VIEWER_HEADERS
    src/Widgets/Viewers/Viewer.hpp
    src/Widgets/Viewers/ImageViewer.hpp
    src/Widgets/Viewers/PlaintextViewer.hpp
    src/Widgets/Viewers/P3DViewer.hpp
)

set(VIEWER_SOURCES
    src/Widgets/Viewers/Viewer.cpp
    src/Widgets/Viewers/ImageViewer.cpp
    src/Widgets/Viewers/PlaintextViewer.cpp
    src/Widgets/Viewers/P3DViewer.cpp
)

set(WIDGET_HEADERS
    src/Widgets/MainWindow.hpp
    src/Widgets/FileInspector.hpp
    ${VIEWER_HEADERS}
)

set(WIDGET_SOURCES
    src/Widgets/MainWindow.cpp
    src/Widgets/FileInspector.cpp
    ${VIEWER_SOURCES}
)

set(FORMATS_HEADERS
    src/Formats/Peg.hpp
    src/Formats/Vpp.hpp
)

set(HEADERS
    src/FrameDecoder.hpp
    src/Util.hpp
	src/Entry.hpp
    src/PegEntry.hpp
    src/VppEntry.hpp
    src/FileViewModel.hpp
    src/ProxyModel.hpp
    ${FORMATS_HEADERS}
    ${READER_HEADERS}
    ${WIDGET_HEADERS}
)

set(SOURCES
    src/Main.cpp
    src/FrameDecoder.cpp
    src/Util.cpp
	src/Entry.cpp
    src/PegEntry.cpp
    src/VppEntry.cpp
    src/FileViewModel.cpp
    src/ProxyModel.cpp
    ${READER_SOURCES}
    ${WIDGET_SOURCES}
    ${HEADERS}
)

set(FORMS
    ui/ImageViewer.ui
    ui/PlaintextViewer.ui
    ui/P3DViewer.ui
)

list(APPEND ACE3X_INCLUDES src)

add_executable(${EXE_NAME}
    ${SOURCES}
    ${FORMS}
)

target_link_libraries(${EXE_NAME} PRIVATE ${ACE3X_LIBS})
target_include_directories(${EXE_NAME} PRIVATE ${ACE3X_INCLUDES})

install(TARGETS ${EXE_NAME} DESTINATION bin)

file(GLOB_RECURSE SOURCES_GLOB *.cpp* *.h* *.hpp*)
file(GLOB_RECURSE FORMS_GLOB *.ui*)

foreach(_source IN ITEMS ${SOURCES_GLOB} ${FORMS_GLOB})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

# Why doesn't this work?
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXE_NAME})
