cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0048 NEW)

project(DowStatsAutorun VERSION 1.0 LANGUAGES CXX)

set(TARGET_NAME DowStatsAutorun)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "DowStatsAutorun_INSTALATION_DIR: " ${DowStatsAutorun_INSTALATION_DIR})
message(STATUS "DowStatsAutorun_LIBRARY_DIR: " ${DowStatsAutorun_LIBRARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DowStatsAutorun_INSTALATION_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DowStatsAutorun_LIBRARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DowStatsAutorun_LIBRARY_DIR})

file(MAKE_DIRECTORY ${DowStatsAutorun_INSTALATION_DIR})
file(MAKE_DIRECTORY ${DowStatsAutorun_LIBRARY_DIR})

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(DEPLOY_DIR ${DowStatsAutorun_INSTALATION_DIR})


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

set(SOURCES_FILES
        main.cpp
        ${SOURCES_FILES}
)

add_executable(${TARGET_NAME} WIN32 ${SOURCES_FILES})

target_link_libraries(${TARGET_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

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
    #--qmldir "${_qt_bin_dir}/../qml"
    ${BUILD_TYPE_ARG}
    $<TARGET_FILE:${TARGET_NAME}>
    COMMENT "Running windeployqt for DowStatsAutorun..."
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

#add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
#    COMMAND "${CMAKE_COMMAND}" -E
#    copy
#    ${_qt_bin_dir}/../qml/Qt/labs/platform/plugins.qmltypes
#    ${DEPLOY_DIR}/Qt/labs/platform/plugins.qmltypes
#)

#add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
#    COMMAND "${CMAKE_COMMAND}" -E
#    copy
#    ${OPENSSL_ROOT_DIR}/bin/libcrypto-1_1.dll
#    ${DEPLOY_DIR}/libcrypto-1_1.dll
#)

#add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
#    COMMAND "${CMAKE_COMMAND}" -E
#    copy
#    ${OPENSSL_ROOT_DIR}/bin/libssl-1_1.dll
#    ${DEPLOY_DIR}/libssl-1_1.dll
#)



