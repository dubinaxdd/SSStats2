cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0048 NEW)

project(DowStats2 VERSION 2.1 LANGUAGES CXX)

#set(PROJECT_VERSION_MAJOR "0")
#set(PROJECT_VERSION_MINOR "1")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(externals/ReplayReaderLibrary)
add_subdirectory(externals/quazip)

set(QUAZIP_USE_QT_ZLIB ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

exec_program(
      "git"
      ${CMAKE_CURRENT_SOURCE_DIR}
      ARGS "rev-list --count main"
      OUTPUT_VARIABLE REL
)
configure_file( "${CMAKE_SOURCE_DIR}/version.h.template" "${CMAKE_SOURCE_DIR}/version.h" )


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick QuickControls2 LinguistTools Network Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick QuickControls2 LinguistTools Network Widgets REQUIRED)

#set(TS_FILES DowStats2_en_GB.ts)


set(PROJECT_SOURCES
        main.cpp
        dev/core/core.h
        dev/core/core.cpp
        dev/core/settingsController/settingscontroller.h
        dev/core/settingsController/settingscontroller.cpp
        dev/core/hookManager/hookmanager.h
        dev/core/hookManager/hookmanager.cpp
        dev/keyboardProcessor/keyboardprocessor.h
        dev/keyboardProcessor/keyboardprocessor.cpp
        dev/uiBackend/uibackend.h
        dev/uiBackend/uibackend.cpp
        dev/SsController/SsController.h
        dev/SsController/SsController.cpp
        dev/SsController/WarningsLogReader/WarningsLogReader.h
        dev/SsController/WarningsLogReader/WarningsLogReader.cpp
        dev/SsController/playerSteamScanner/playerssteamscanner.h
        dev/SsController/playerSteamScanner/playerssteamscanner.cpp
        dev/SsController/memoryController/memorycontroller.h
        dev/SsController/memoryController/memorycontroller.cpp
        dev/SsController/statsCollector/statscollector.h
        dev/SsController/statsCollector/statscollector.cpp
        dev/SsController/apmMeter/apmmeter.h
        dev/SsController/apmMeter/apmmeter.cpp
        dev/SsController/lobbyEventReader/lobbyeventreader.h
        dev/SsController/lobbyEventReader/lobbyeventreader.cpp
        dev/SsController/dowServerProcessor/DowServerProcessor.h
        dev/SsController/dowServerProcessor/DowServerProcessor.cpp
        dev/uiBackend/statisticPanel/statisticpanel.h
        dev/uiBackend/statisticPanel/statisticpanel.cpp
        dev/uiBackend/gamePanel/gamepanel.h
        dev/uiBackend/gamePanel/gamepanel.cpp
        dev/uiBackend/imageProvider/imageprovider.h
        dev/uiBackend/imageProvider/imageprovider.cpp
        dev/baseTypes/baseTypes.h
        dev/uiBackend/statisticPanel/statisticpanelitem.h
        dev/uiBackend/statisticPanel/statisticpanelitem.cpp
        dev/core/logger/logger.h
        dev/core/logger/logger.cpp
        dev/discordController/discordController.h
        dev/discordController/discordController.cpp
        dev/uiBackend/MessagesPage/MessagesPage.h
        dev/uiBackend/MessagesPage/MessagesPage.cpp
        dev/ModsProcessor/ModsProcessor.h
        dev/ModsProcessor/ModsProcessor.cpp
        dev/ModsProcessor/ModsDownloader/ModsDownloader.h
        dev/ModsProcessor/ModsDownloader/ModsDownloader.cpp
        dev/ModsProcessor/ModsInstaller/ModsInstaller.h
        dev/ModsProcessor/ModsInstaller/ModsInstaller.cpp
        dev/uiBackend/SettingsPage/SettingsPageModel.h
        dev/uiBackend/SettingsPage/SettingsPageModel.cpp


        defines.h
        qml.qrc
        resources.qrc
)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/qt5app.rc")


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DowStats2 WIN32
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${APP_ICON_RESOURCE_WINDOWS}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(DowStats2 SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(DowStats2 WIN32
          ${PROJECT_SOURCES}
          ${APP_ICON_RESOURCE_WINDOWS}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()


#set( OPENSSL_ROOT_DIR "C:/Qt/Tools/OpenSSL/Win_x86")

find_package(OpenSSL)
if (OPENSSL_FOUND)
    message (STATUS "OPENSSL found")
    message (STATUS "OpenSSL INclude directories:" ${OPENSSL_INCLUDE_DIR})
else()
message (FATAL_ERROR "OpenSSL Not found.")
    endif()


target_compile_definitions(DowStats2
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(DowStats2
                        PRIVATE
                        Qt${QT_VERSION_MAJOR}::Core
                        Qt${QT_VERSION_MAJOR}::Quick
                        Qt${QT_VERSION_MAJOR}::QuickControls2
                        Qt${QT_VERSION_MAJOR}::Network
                        Qt${QT_VERSION_MAJOR}::Widgets
                        OpenSSL::SSL
                        OpenSSL::Crypto
                        QuaZip::QuaZip
                        ReplayReaderLibrary)


set_target_properties(DowStats2 PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(DowStats2)
    qt_finalize_executable(DowStats2)
endif()


if(CMAKE_BUILD_TYPE STREQUAL Release)

    set(WINSDK_MT_DIR "${WINSDK_MT_DIR}")

    if("${WINSDK_MT_DIR}" STREQUAL "")
        message (FATAL_ERROR "WinSDK mt.exe path is empty, please set cmake option DWINSDK_MT_DIR, for example: \"-DWINSDK_MT_DIR:=C:/Windows Kits/10/bin/10.0.22000.0/x86/\".")
    else()
        message (STATUS "WINSDK_MT_DIR: " ${WINSDK_MT_DIR})
        message(STATUS "Build dir: " ${CMAKE_BINARY_DIR})
    endif()

    add_custom_command(
        TARGET DowStats2
        POST_BUILD
        COMMAND ${WINSDK_MT_DIR}mt.exe -manifest ${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml -outputresource:${CMAKE_BINARY_DIR}/DowStats2.exe
        COMMENT "Adding administator manifest."
    )
endif()

include_directories(DowStats2
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/core
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/core/settingsController
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/core/hookManager
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/keyboardProcessor
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/SsController
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/SsController/WarningsLogReader
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/SsController/playerSteamScanner
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/SsController/memoryController
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/SsController/statsCollector
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/SsController/apmMeter
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/SsController/lobbyEventReader
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/SsController/dowServerProcessor
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/statisticPanel
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/gamePanel
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/imageProvider
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/baseTypes
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/statisticPanel
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/core/logger    
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/ReplayReaderLibrary
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/discordController
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/MessagesPage
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/ModsProcessor
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/ModsProcessor/ModsDownloader
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/ModsProcessor/ModsInstaller
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/SettingsPage/
)
