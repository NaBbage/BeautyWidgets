cmake_minimum_required(VERSION 3.16)

project(BeautyWidgetsDemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(BEAUTY_WIDGETS_SOURCES
        src/beautylineedit.cpp
        src/beautylineedit.h
        src/beautypushbutton.cpp
        src/beautypushbutton.h
)

add_library(BeautyWidgets STATIC
        ${BEAUTY_WIDGETS_SOURCES}
)

target_include_directories(BeautyWidgets
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(BeautyWidgets
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
)

set(BEAUTY_WIDGETS_DEMO_SOURCES
        main.cpp
        beautywidgetsdemo.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BeautyWidgetsDemo
        MANUAL_FINALIZATION
        ${BEAUTY_WIDGETS_DEMO_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BeautyWidgetsDemo APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(BeautyWidgetsDemo SHARED
            ${BEAUTY_WIDGETS_DEMO_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BeautyWidgetsDemo
            ${BEAUTY_WIDGETS_DEMO_SOURCES}
        )
    endif()
endif()

target_link_libraries(BeautyWidgetsDemo PRIVATE BeautyWidgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BeautyWidgetsDemo)
endif()
set_target_properties(BeautyWidgetsDemo PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS BeautyWidgetsDemo
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BeautyWidgetsDemo)
endif()
