cmake_minimum_required(VERSION 3.21.0)

#set PROJECT_NAME and version
project(IMG_FUSION)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)

#set path for QT
set(CMAKE_PREFIX_PATH C:\\Qt\\Qt5.14.2\\5.14.2\\msvc2017_64)

file(GLOB_RECURSE QRC_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

set(QRC_SOURCE_FILES rc.qrc)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)
find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})

file(GLOB SOURCE_FILE 
	*.cpp
	FileOperation/*.cpp
	FusionAlgorithm/*.cpp
	UI/*.cpp)
file(GLOB HEAD_FILE 
	*.h
	FileOperation/*.h
	FusionAlgorithm/*.h
	UI/*.h)

set(TS_FILE zh_CN.ts)
set(RC_FILE res/logo.rc)

add_executable(${PROJECT_NAME} ${QRC_FILES} ${SOURCE_FILE} ${HEAD_FILE} ${TS_FILE} ${RC_FILE})

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILE})

# Use the widgets module from Qt 5
target_link_libraries(${PROJECT_NAME}
	PUBLIC
	Qt5::Core
	Qt5::Gui
	Qt5::Widgets)

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/installed)
set(WINDEPLOYQT_DIR C:\\Qt\\Qt5.14.2\\5.14.2\\msvc2017_64\\bin)
#exec windeployqt automatically when build install in VS
install(CODE "execute_process(COMMAND ${WINDEPLOYQT_DIR}/windeployqt.exe 
				${PROJECT_BINARY_DIR}/Release/${PROJECT_NAME}.exe)")
install(DIRECTORY ${PROJECT_BINARY_DIR}/Release/ DESTINATION /)

set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})

INCLUDE(CPack)