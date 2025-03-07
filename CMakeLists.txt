cmake_minimum_required(VERSION 2.8.12)

project(sighandlers)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS YES)

file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

if(BUILD_SHARED_LIBS)
add_library(${PROJECT_NAME} SHARED ${SRCS})
else()
add_library(${PROJECT_NAME} STATIC ${SRCS})
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(WIN32)
target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

if (APPLE)
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_NAME_DIR "@rpath")
endif()

add_executable(${PROJECT_NAME}_sah_test "${CMAKE_CURRENT_SOURCE_DIR}/src/test/sah.cpp")
target_include_directories(${PROJECT_NAME}_sah_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_sah_test ${PROJECT_NAME})

add_executable(${PROJECT_NAME}_sfh_test "${CMAKE_CURRENT_SOURCE_DIR}/src/test/sfh.cpp")
target_include_directories(${PROJECT_NAME}_sfh_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_sfh_test ${PROJECT_NAME})

if(WIN32)
target_link_libraries(${PROJECT_NAME} ws2_32)
else()
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
endif()
