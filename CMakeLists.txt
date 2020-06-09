cmake_minimum_required(VERSION 3.0)

project("gemctrl" LANGUAGES CXX)

set(SOURCES src/main.cpp src/gemini.h src/gemini.cpp)

find_package(wxWidgets COMPONENTS core base qa adv net html propgrid richtext REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable( gemctrl ${SOURCES})
target_link_libraries(gemctrl ${wxWidgets_LIBRARIES})

