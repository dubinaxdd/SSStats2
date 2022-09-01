cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0048 NEW)

project(ReplayReaderLibrary VERSION 1.0 LANGUAGES CXX)
set(TARGET_NAME ReplayReaderLibrary)

#set(PROJECT_VERSION_MAJOR "1")
#set(PROJECT_VERSION_MINOR "0")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick QuickControls2 LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick QuickControls2 LinguistTools REQUIRED)


set(PROJECT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/extendedbinreader.h
        ${CMAKE_CURRENT_SOURCE_DIR}/extendedbinreader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/game_action.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gamesettings.h
        ${CMAKE_CURRENT_SOURCE_DIR}/gamesettings.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/player.h
        ${CMAKE_CURRENT_SOURCE_DIR}/player.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/replay.h
        ${CMAKE_CURRENT_SOURCE_DIR}/replay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/repreader.h
        ${CMAKE_CURRENT_SOURCE_DIR}/repreader.cpp
)

add_library(${TARGET_NAME} STATIC ${PROJECT_SOURCES})

target_link_libraries(${TARGET_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
