cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0048 NEW)

project(SSStats2 VERSION 2.0 LANGUAGES CXX)

#set(PROJECT_VERSION_MAJOR "0")
#set(PROJECT_VERSION_MINOR "1")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package(...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

exec_program(
      "git"
      ${CMAKE_CURRENT_SOURCE_DIR}
      ARGS "rev-list --count main"
      OUTPUT_VARIABLE REL
)
configure_file( "${CMAKE_SOURCE_DIR}/version.h.template" "${CMAKE_SOURCE_DIR}/version.h" )


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick QuickControls2 LinguistTools Network Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick QuickControls2 LinguistTools Network Widgets REQUIRED)

#set(TS_FILES SSStats2_en_GB.ts)


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
        dev/ssConroller/sscontroller.h
        dev/ssConroller/sscontroller.cpp
        dev/ssConroller/gameInfoReader/gameinforeader.h
        dev/ssConroller/gameInfoReader/gameinforeader.cpp
        dev/ssConroller/playerSteamScanner/playerssteamscanner.h
        dev/ssConroller/playerSteamScanner/playerssteamscanner.cpp
        dev/ssConroller/memoryController/memorycontroller.h
        dev/ssConroller/memoryController/memorycontroller.cpp
        dev/ssConroller/statsCollector/statscollector.h
        dev/ssConroller/statsCollector/statscollector.cpp
        dev/ssConroller/apmMeter/apmmeter.h
        dev/ssConroller/apmMeter/apmmeter.cpp
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
        dev/ssConroller/lobbyEventReader/lobbyeventreader.h
        dev/ssConroller/lobbyEventReader/lobbyeventreader.cpp


        dev/ssConroller/gameInfoReader/repReader/extendedbinreader.h
        dev/ssConroller/gameInfoReader/repReader/extendedbinreader.cpp
        dev/ssConroller/gameInfoReader/repReader/game_action.h
        dev/ssConroller/gameInfoReader/repReader/gamesettings.h
        dev/ssConroller/gameInfoReader/repReader/gamesettings.cpp
        dev/ssConroller/gameInfoReader/repReader/player.h
        dev/ssConroller/gameInfoReader/repReader/player.cpp
        dev/ssConroller/gameInfoReader/repReader/replay.h
        dev/ssConroller/gameInfoReader/repReader/replay.cpp
        dev/ssConroller/gameInfoReader/repReader/repreader.h
        dev/ssConroller/gameInfoReader/repReader/repreader.cpp

        defines.h
        qml.qrc
        resources.qrc
       # ${TS_FILES}
)





set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/qt5app.rc")


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SSStats2 WIN32
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${APP_ICON_RESOURCE_WINDOWS}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(SSStats2 SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(SSStats2 WIN32
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


target_compile_definitions(SSStats2
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(SSStats2
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickControls2 Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Widgets OpenSSL::SSL OpenSSL::Crypto)


set_target_properties(SSStats2 PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(SSStats2)
    qt_finalize_executable(SSStats2)
endif()

set(WINSDK_MT_DIR "${WINSDK_MT_DIR}")



if("${WINSDK_MT_DIR}" STREQUAL "")
    message (FATAL_ERROR "WinSDK mt.exe path is empty, please set cmake option DWINSDK_MT_DIR, for example: \"-DWINSDK_MT_DIR:=C:/Windows Kits/10/bin/10.0.22000.0/x86/\".")
else()
    message (STATUS "WINSDK_MT_DIR: " ${WINSDK_MT_DIR})
    message(STATUS "Build dir: " ${CMAKE_BINARY_DIR})


endif()

add_custom_command(
        TARGET SSStats2
        POST_BUILD
        COMMAND ${WINSDK_MT_DIR}mt.exe -manifest ${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml -outputresource:${CMAKE_BINARY_DIR}/SSStats2.exe
        COMMENT "Adding administator manifest."
)


include_directories(SSStats2
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/core
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/core/settingsController
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/core/hookManager
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/keyboardProcessor
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/ssConroller
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/ssConroller/gameInfoReader
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/ssConroller/playerSteamScanner
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/ssConroller/memoryController
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/ssConroller/statsCollector
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/ssConroller/apmMeter
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/statisticPanel
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/gamePanel
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/imageProvider
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/baseTypes
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/uiBackend/statisticPanel
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/core/logger
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/ssConroller/lobbyEventReader
    ${CMAKE_CURRENT_SOURCE_DIR}/dev/ssConroller/gameInfoReader/repReader
)
