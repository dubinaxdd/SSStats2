cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0048 NEW)

project(DowStats2 VERSION 2.2 LANGUAGES CXX)

set(TARGET_NAME DowStats2)

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


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick QuickControls2 LinguistTools Network Widgets Multimedia REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick QuickControls2 LinguistTools Network Widgets Multimedia REQUIRED)

#set(TS_FILES DowStats2_en_GB.ts)


set(PROJECT_SOURCES
        main.cpp
        dev/core.h
        dev/core.cpp
        dev/settingsController/settingsController.h
        dev/settingsController/settingsController.cpp
        dev/hookManager/hookManager.h
        dev/hookManager/hookManager.cpp
        dev/keyboardProcessor/keyboardProcessor.h
        dev/keyboardProcessor/keyboardProcessor.cpp
        dev/uiBackend/uiBackend.h
        dev/uiBackend/uiBackend.cpp
        dev/soulstormController/soulstormController.h
        dev/soulstormController/soulstormController.cpp
        dev/soulstormController/gameStateReader/gameStateReader.h
        dev/soulstormController/gameStateReader/gameStateReader.cpp
        dev/soulstormController/soulstormMemoryReader/soulstormMemoryReader.h
        dev/soulstormController/soulstormMemoryReader/soulstormMemoryReader.cpp
        dev/soulstormController/soulstormMemoryController/soulstormMemoryController.h
        dev/soulstormController/soulstormMemoryController/soulstormMemoryController.cpp
        dev/soulstormController/apmMeter/apmMeter.h
        dev/soulstormController/apmMeter/apmMeter.cpp
        dev/soulstormController/lobbyEventReader/lobbyEventReader.h
        dev/soulstormController/lobbyEventReader/lobbyEventReader.cpp
        dev/soulstormController/dowServerProcessor/dowServerProcessor.h
        dev/soulstormController/dowServerProcessor/dowServerProcessor.cpp
        dev/statsServerProcessor/statsServerProcessor.h
        dev/statsServerProcessor/statsServerProcessor.cpp
        dev/uiBackend/statisticPanel/statisticPanel.h
        dev/uiBackend/statisticPanel/statisticPanel.cpp
        dev/uiBackend/statisticPanel/statisticPanelItem.h
        dev/uiBackend/statisticPanel/statisticPanelItem.cpp
        dev/uiBackend/imageProvider/imageProvider.h
        dev/uiBackend/imageProvider/imageProvider.cpp
        dev/uiBackend/gamePanel/gamePanel.h
        dev/uiBackend/gamePanel/gamePanel.cpp
        dev/baseTypes/baseTypes.h
        dev/logger/logger.h
        dev/logger/logger.cpp
        dev/discordWebProcessor/discordWebProcessor.h
        dev/discordWebProcessor/discordWebProcessor.cpp
        dev/uiBackend/messagesPage/messagesPage.h
        dev/uiBackend/messagesPage/messagesPage.cpp
        dev/modsProcessor/modsProcessor.h
        dev/modsProcessor/modsProcessor.cpp
        dev/modsProcessor/modsDownloader/modsDownloader.h
        dev/modsProcessor/modsDownloader/modsDownloader.cpp
        dev/modsProcessor/modsInstaller/modsInstaller.h
        dev/modsProcessor/modsInstaller/modsInstaller.cpp
        dev/uiBackend/settingsPage/settingsPageModel.h
        dev/uiBackend/settingsPage/settingsPageModel.cpp
        dev/soundProcessor/soundProcessor.h
        dev/soundProcessor/soundProcessor.cpp
        dev/soulstormController/replayDataCollector/replayDataCollector.h
        dev/soulstormController/replayDataCollector/replayDataCollector.cpp
        dev/overlayWindowController/overlayWindowController.h
        dev/overlayWindowController/overlayWindowController.cpp

        defines.h
        qml.qrc
        resources.qrc
)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/qt5app.rc")


add_executable(${TARGET_NAME} WIN32 ${PROJECT_SOURCES} ${APP_ICON_RESOURCE_WINDOWS})

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

#set( OPENSSL_ROOT_DIR "C:/Qt/Tools/OpenSSL/Win_x86")

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
                        OpenSSL::SSL
                        OpenSSL::Crypto
                        QuaZip::QuaZip
                        ReplayReaderLibrary)

if(CMAKE_BUILD_TYPE STREQUAL Release)

    set(WINSDK_MT_DIR "${WINSDK_MT_DIR}")

    if("${WINSDK_MT_DIR}" STREQUAL "")
        message (FATAL_ERROR "WinSDK mt.exe path is empty, please set cmake option DWINSDK_MT_DIR, for example: \"-DWINSDK_MT_DIR:=C:/Windows Kits/10/bin/10.0.22000.0/x86/\".")
    else()
        message (STATUS "WINSDK_MT_DIR: " ${WINSDK_MT_DIR})
        message(STATUS "Build dir: " ${CMAKE_BINARY_DIR})
    endif()

    add_custom_command(
        TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${WINSDK_MT_DIR}mt.exe -manifest ${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml -outputresource:${CMAKE_BINARY_DIR}/DowStats2.exe
        COMMENT "Adding administator manifest."
    )
endif()

include_directories(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/dev
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/baseTypes
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/settingsController
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/hookManager
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/keyboardProcessor
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/discordWebProcessor
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/soundProcessor
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/logger
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/statsServerProcessor
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/overlayWindowController
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/soulstormController
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/soulstormController/gameStateReader
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/soulstormController/soulstormMemoryReader
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/soulstormController/soulstormMemoryController
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/soulstormController/apmMeter
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/soulstormController/lobbyEventReader
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/soulstormController/dowServerProcessor
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/soulstormController/replayDataCollector
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/soulstormController/replayDataCollector
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/statisticPanel
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/gamePanel
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/imageProvider
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/settingsPage
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/messagesPage
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/statisticPanel
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/modsProcessor
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/modsProcessor/modsDownloader
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/modsProcessor/modsInstaller
)
