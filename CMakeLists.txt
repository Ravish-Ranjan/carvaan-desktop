cmake_minimum_required(VERSION 3.20)

project(carvaan-desktop
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(WEBKITGTK REQUIRED webkit2gtk-4.1)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(LIBSOUP REQUIRED libsoup-3.0)

add_executable(carvaan-desktop
    src/main.cpp

    src/webview/WebViewApp.cpp
    
    src/core/Database.cpp
    src/core/AppPaths.cpp
    src/core/Song.cpp
    src/core/SongRepository.cpp
    src/core/LibraryManager.cpp
    src/core/AdbDeviceRepository.cpp
    src/core/SettingsRepository.cpp
    
    src/utils/Json.cpp

    src/features/library/LibraryFeatures.cpp
    src/features/configure/ConfigureFeatures.cpp
    src/features/adb/AdbManager.cpp
    src/features/player/PlayerFeatures.cpp
)

target_precompile_headers(carvaan-desktop PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include/json/json.hpp
)

target_include_directories(carvaan-desktop PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GTK3_INCLUDE_DIRS}
    ${WEBKITGTK_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIRS}
    ${GSTREAMER_INCLUDE_DIRS}
    ${LIBSOUP_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_directories(carvaan-desktop PRIVATE
    ${GTK3_LIBRARY_DIRS}
    ${WEBKITGTK_LIBRARY_DIRS}
    ${SQLITE3_LIBRARY_DIRS}
    ${GSTREAMER_LIBRARY_DIRS}
    ${LIBSOUP_LIBRARY_DIRS}
)

target_compile_options(carvaan-desktop PRIVATE
    ${GTK3_CFLAGS_OTHER}
    ${WEBKITGTK_CFLAGS_OTHER}
    ${SQLITE3_CFLAGS_OTHER}
    ${GSTREAMER_CFLAGS_OTHER}
    ${LIBSOUP_CFLAGS_OTHER}
)

target_link_libraries(carvaan-desktop PRIVATE
    ${GTK3_LIBRARIES}
    ${WEBKITGTK_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    ${GSTREAMER_LIBRARIES}
    ${LIBSOUP_LIBRARIES}
)

# ---------------------------------------------------------------------------
# Installation

include(GNUInstallDirs)

add_compile_definitions(
    SYNC_SONGS_FRONTEND_DIR="${CMAKE_INSTALL_FULL_DATADIR}/carvaan-desktop/frontend"
)

install(
    TARGETS carvaan-desktop
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY resources/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/carvaan-desktop
)

install(
    DIRECTORY frontend/dist/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/carvaan-desktop/frontend
)

# ---------------------------------------------------------------------------
# Debian package

set(CPACK_GENERATOR "DEB")

set(CPACK_PACKAGE_NAME "carvaan-desktop")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Saregama Carvaan music synchronization manager"
)
set(CPACK_PACKAGE_DESCRIPTION
    "A desktop application for managing and synchronizing music between a local library and Android devices."
)

set(CPACK_PACKAGE_CONTACT "Sync Songs")
set(CPACK_PACKAGE_VENDOR "Sync Songs")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sync Songs")

set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}_${PROJECT_VERSION}_${CMAKE_SYSTEM_PROCESSOR}"
)

include(CPack)