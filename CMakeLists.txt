cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0048 NEW)

project(DowStatsClient VERSION 2.5 LANGUAGES CXX)

set(TARGET_NAME DowStatsClient)

#set(PROJECT_VERSION_MAJOR "0")
#set(PROJECT_VERSION_MINOR "1")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QUAZIP_USE_QT_ZLIB ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

#set(BALANCE_MOD_TEST OFF CACHE BOOL "")

option(BALANCE_MOD_TEST "Build with DoW Stats Balance Mod TEST server" OFF)
if (${BALANCE_MOD_TEST})
    add_definitions(-DBALANCE_MOD_TEST)
    message(STATUS "Build with DoW Stats Balance Mod TEST server")
endif()


option(TEST_NEWS_CHANNEL "Build with DoW Stats News test channel" OFF)
if (${TEST_NEWS_CHANNEL})
    add_definitions(-DTEST_NEWS_CHANNEL)
    message(STATUS "Build with DoW Stats News test channel")
endif()


option(TEST_DISCORD_MICROSERVICE "Build with local test discord microservice" OFF)
if (${TEST_DISCORD_MICROSERVICE})
    add_definitions(-DTEST_DISCORD_MICROSERVICE)
    message(STATUS "Build with local test discord microservice")
endif()

option(DISAPLE_HOOK_FOR_DEBUG "Disable hook for debug without lags" OFF)
if (${DISAPLE_HOOK_FOR_DEBUG})
    add_definitions(-DDISAPLE_HOOK_FOR_DEBUG)
    message(STATUS "Disable hook for debug without lags")
endif()


add_subdirectory(externals/ReplayReaderLibrary)
add_subdirectory(externals/quazip)

set(DowStatsClient_INSTALATION_DIR  ${PROJECT_BINARY_DIR}/bin)
set(DowStatsClient_LIBRARY_DIR  ${PROJECT_BINARY_DIR}/lib)

set(DowStatsAutorun_INSTALATION_DIR  ${DowStatsClient_INSTALATION_DIR})
set(DowStatsAutorun_LIBRARY_DIR  ${DowStatsClient_LIBRARY_DIR})

message(STATUS "DowStatsClient_INSTALATION_DIR: " ${DowStatsClient_INSTALATION_DIR})
message(STATUS "DowStatsClient_LIBRARY_DIR: " ${DowStatsClient_LIBRARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DowStatsClient_INSTALATION_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DowStatsClient_LIBRARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DowStatsClient_LIBRARY_DIR})

file(MAKE_DIRECTORY ${DowStatsClient_INSTALATION_DIR})
file(MAKE_DIRECTORY ${DowStatsClient_LIBRARY_DIR})

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(DEPLOY_DIR ${DowStatsClient_INSTALATION_DIR})

add_subdirectory(autoRun)

exec_program(
      "git"
      ${CMAKE_CURRENT_SOURCE_DIR}
      ARGS "rev-list --count main"
      OUTPUT_VARIABLE REL
)
configure_file( "${CMAKE_SOURCE_DIR}/version.h.template" "${CMAKE_SOURCE_DIR}/version.h" )


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick QuickControls2 LinguistTools Network Widgets Multimedia WebSockets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick QuickControls2 LinguistTools Network Widgets Multimedia WebSockets REQUIRED)

set (LANGUAGES "ru_RU")
foreach (LANGUAGE ${LANGUAGES})
    set (TS ${CMAKE_CURRENT_SOURCE_DIR}/resources/translations/DowStatsClient_${LANGUAGE}.ts)
    set (QM ${CMAKE_CURRENT_SOURCE_DIR}/resources/translations/DowStatsClient_${LANGUAGE}.qm)
    add_custom_command ( OUTPUT ${QM} COMMAND ${Qt5_LRELEASE_EXECUTABLE} ${TS} -qm ${QM} MAIN_DEPENDENCY ${TS} COMMENT "Generating translations")
    #qt5_add_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/translations ${TS})
endforeach()

file(GLOB_RECURSE HEADERS_FILES dev/*.h)
file(GLOB_RECURSE SOURCES_FILES dev/*.cpp)

set(SOURCES_FILES
        main.cpp
        defines.h
        qml.qrc
        resources.qrc
        resources/qml/qmldir
        ${SOURCES_FILES}
)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/qt5app.rc")

add_executable(${TARGET_NAME} WIN32 ${HEADERS_FILES} ${SOURCES_FILES} ${APP_ICON_RESOURCE_WINDOWS})

#set( OPENSSL_ROOT_DIR "C:/Qt/Tools/OpenSSL/Win_x64")

find_package(OpenSSL)
if (OPENSSL_FOUND)
    message (STATUS "OPENSSL found")
    message (STATUS "OpenSSL INclude directories:" ${OPENSSL_INCLUDE_DIR})
else()
message (FATAL_ERROR "OpenSSL Not found.")
    endif()


target_compile_definitions(${TARGET_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${TARGET_NAME}
                        PRIVATE
                        Qt${QT_VERSION_MAJOR}::Core
                        Qt${QT_VERSION_MAJOR}::Quick
                        Qt${QT_VERSION_MAJOR}::QuickControls2
                        Qt${QT_VERSION_MAJOR}::Network
                        Qt${QT_VERSION_MAJOR}::Widgets
                        Qt${QT_VERSION_MAJOR}::Multimedia
                        Qt${QT_VERSION_MAJOR}::WebSockets
                        OpenSSL::SSL
                        OpenSSL::Crypto
                        QuaZip::QuaZip
                        ReplayReaderLibrary)


if(CMAKE_BUILD_TYPE STREQUAL Release)

    set(WINSDK_MT_DIR ${WINSDK_MT_DIR})

    if(${WINSDK_MT_DIR} STREQUAL "")
        message (FATAL_ERROR "WinSDK mt.exe path is empty, please set cmake option -DWINSDK_MT_DIR, for example: \"-DWINSDK_MT_DIR:=C:/Windows Kits/10/bin/10.0.22000.0/x86/\".")
    else()
        message (STATUS "WINSDK_MT_DIR: " ${WINSDK_MT_DIR})
        message(STATUS "Build dir: " ${CMAKE_BINARY_DIR})
    endif()

    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${WINSDK_MT_DIR}/mt.exe -manifest ${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml -outputresource:${DowStatsClient_INSTALATION_DIR}/DowStatsClient.exe
        COMMENT "Adding administator manifest."
    )

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
        remove
        ${DEPLOY_DIR}/DowStatsClient.exe.manifest
    )

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
        remove
        ${DEPLOY_DIR}/DowStatsAutorun.exe.manifest
    )

endif()

file(GLOB  dev_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dev ${CMAKE_CURRENT_SOURCE_DIR}/dev/*)
file(GLOB  gameController_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dev/gameController ${CMAKE_CURRENT_SOURCE_DIR}/dev/gameController/*)
file(GLOB  mapManager_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dev/mapManager ${CMAKE_CURRENT_SOURCE_DIR}/dev/mapManager/*)
file(GLOB  uiBackend_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/*)
file(GLOB  modsProcessor_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dev/modsProcessor ${CMAKE_CURRENT_SOURCE_DIR}/dev/modsProcessor/*)
file(GLOB  replayManager_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/replayManager ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/replayManager/*)
file(GLOB  modsPage_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/modsPage ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/modsPage/*)
file(GLOB  balanceModManager_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dev/balanceModManager ${CMAKE_CURRENT_SOURCE_DIR}/dev/balanceModManager/*)

set(SOURCE_DIRECTORIES
    ${dev_DIR}
    ${gameController_DIR}
    ${mapManager_DIR}
    ${uiBackend_DIR}
    ${modsProcessor_DIR}
    ${replayManager_DIR}
    ${modsPage_DIR}
    ${balanceModManager_DIR}
)

#message(STATUS "SOURCE_DIRECTORIES: " ${SOURCE_DIRECTORIES})

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SOURCE_DIRECTORIES}
)

install(TARGETS ${TARGET_NAME} DESTINATION .)

get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")


if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(BUILD_TYPE_ARG --debug)
    set(PLATFORM_LIB_NAME qtlabsplatformplugind.dll)
else()
    set(BUILD_TYPE_ARG --release)
    set(PLATFORM_LIB_NAME qtlabsplatformplugin.dll)
endif()


add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
    env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
    ARGS --no-translations
    --compiler-runtime
    --plugindir "${DEPLOY_DIR}/plugins"
    --dir "${DEPLOY_DIR}"
    --libdir "${DEPLOY_DIR}"
    --qmldir "${_qt_bin_dir}/../qml"
    ${BUILD_TYPE_ARG}
    $<TARGET_FILE:${TARGET_NAME}>
    COMMENT "Running windeployqt for DowStatsClient..."
)


add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
    copy
    ${_qt_bin_dir}/../qml/Qt/labs/platform/${PLATFORM_LIB_NAME}
    ${DEPLOY_DIR}/Qt/labs/platform/${PLATFORM_LIB_NAME}
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
    copy
    ${_qt_bin_dir}/../qml/Qt/labs/platform/qmldir
    ${DEPLOY_DIR}/Qt/labs/platform/qmldir
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
    copy
    ${_qt_bin_dir}/../qml/Qt/labs/platform/plugins.qmltypes
    ${DEPLOY_DIR}/Qt/labs/platform/plugins.qmltypes
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
    copy
    ${OPENSSL_ROOT_DIR}/bin/libcrypto-1_1-x64.dll
    ${DEPLOY_DIR}/libcrypto-1_1-x64.dll
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
    copy
    ${OPENSSL_ROOT_DIR}/bin/libssl-1_1-x64.dll
    ${DEPLOY_DIR}/libssl-1_1-x64.dll
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
    copy
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/bin/updater/Downloader.exe
    ${DEPLOY_DIR}/Downloader.exe
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
    copy
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/bin/updater/Updater.exe
    ${DEPLOY_DIR}/Updater.exe
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
    copy
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/bin/updater/Updater.ini
    ${DEPLOY_DIR}/Updater.ini
)


