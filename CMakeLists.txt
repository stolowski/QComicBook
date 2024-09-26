CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(QComicBook)
SET(VERSION "0.9.1")
SET(PACKAGE qcomicbook) 
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/src/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ELSE()
    ADD_DEFINITIONS( -DDEBUG )
ENDIF()

IF(CMAKE_BUILD_TYPE STREQUAL "Release")
    ADD_DEFINITIONS( -DQT_NO_DEBUG_OUTPUT )
ENDIF()

#set(CMAKE_AUTOMOC ON)

SET(QT_MIN_VERSION "5.4.0")
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5LinguistTools REQUIRED)
FIND_PACKAGE(Qt5PrintSupport REQUIRED)
FIND_PACKAGE(Qt5X11Extras)
#SET(QT_USE_QTOPENGL TRUE)
#INCLUDE(${QT_USE_FILE})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
include_directories(${Qt5X11Extras_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

INCLUDE(GNUInstallDirs)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(POPPLER poppler-qt5>=0.12.4 REQUIRED)

SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${PACKAGE}-${VERSION}")
SET(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_SOURCE_IGNORE_FILES
 "\\\\.git.*"
 "/install_manifest\\\\.txt"
 "CMakeFiles/"
 "Makefile"
 "CMakeCache"
 "cmake_install\\\\.cmake"
 "CPack*" "~$" "\\\\.o$"
 "moc_.*"
 "ui_.+"
 "\\\\.qm$"
 "\\\\.cxx$"
 "/config\\\\.h$"
 "/qcomicbook$")
INCLUDE(CPack)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(help)
ADD_SUBDIRECTORY(i18n)

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})
