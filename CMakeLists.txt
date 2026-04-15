CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

PROJECT(QComicBook)
SET(VERSION "0.9.2")
SET(PACKAGE qcomicbook)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/src/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

IF(NOT CMAKE_BUILD_TYPE)
    #_cmake_modify_IGNORE     SET(CMAKE_BUILD_TYPE Release)
ELSE()
    ADD_DEFINITIONS(-DDEBUG)
ENDIF()

IF(CMAKE_BUILD_TYPE STREQUAL "Release")
    ADD_DEFINITIONS(-DQT_NO_DEBUG_OUTPUT)
ENDIF()

#set(CMAKE_AUTOMOC ON)

SET(QT_MIN_VERSION "6.0.0")
set(QT_NO_PRIVATE_MODULE_WARNING on)
find_package(Qt6 REQUIRED COMPONENTS
    Core
    GuiPrivate
    Widgets
    LinguistTools
    PrintSupport
)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

#SET(QT_USE_QTOPENGL TRUE)
#INCLUDE(${QT_USE_FILE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(POPPLER poppler-qt6>=0.12.4 REQUIRED)

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

message(STATUS "<<< Gentoo configuration >>>
Build type      ${CMAKE_BUILD_TYPE}
Install path    ${CMAKE_INSTALL_PREFIX}
Compiler flags:
C               ${CMAKE_C_FLAGS}
C++             ${CMAKE_CXX_FLAGS}
Linker flags:
Executable      ${CMAKE_EXE_LINKER_FLAGS}
Module          ${CMAKE_MODULE_LINKER_FLAGS}
Shared          ${CMAKE_SHARED_LINKER_FLAGS}\n")
