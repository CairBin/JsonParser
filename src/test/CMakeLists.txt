project(Application)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} APP_SRC)
include_directories(${CMAKE_SOURCE_DIR}/include)

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME} ${APP_SRC})

target_link_libraries(${PROJECT_NAME} JsonParser)

