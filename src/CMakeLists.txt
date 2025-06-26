cmake_minimum_required(VERSION 3.16)

project(inspekcje VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(inspekcje
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        loginwindow.h loginwindow.cpp loginwindow.ui
        backendclient.h backendclient.cpp
        backendclient.h
        backendclient.cpp
        backendclient.h backendclient.cpp
        inspectorwindow.h inspectorwindow.cpp inspectorwindow.ui
        managerwindow.h managerwindow.cpp managerwindow.ui

        monument.h
        monument.h
        jwtmanager.h jwtmanager.cpp
        monument.cpp
        inspectionorderdialog.h inspectionorderdialog.cpp inspectionorderdialog.ui
        selectuserdialog.h selectuserdialog.cpp selectuserdialog.ui
        res.qrc
        inspectionrow.h inspectionrow.cpp inspectionrow.ui
        dialogeditinspection.h dialogeditinspection.cpp dialogeditinspection.ui
        dialogshowreport.h dialogshowreport.cpp dialogshowreport.ui
        dialogcompleteinspection.h dialogcompleteinspection.cpp dialogcompleteinspection.ui
        inspectionrowforinspector.h inspectionrowforinspector.cpp inspectionrowforinspector.ui
        dialogsettings.h dialogsettings.cpp dialogsettings.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET inspekcje APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(inspekcje SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(inspekcje
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(inspekcje PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.inspekcje)
endif()
set_target_properties(inspekcje PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS inspekcje
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(inspekcje)
endif()
