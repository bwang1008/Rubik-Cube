SET(pwd ${CMAKE_CURRENT_LIST_DIR})

SET(src "")
AUX_SOURCE_DIRECTORY(${pwd} src)
add_executable(animation ${src})
message(STATUS "animation added ${src}")

target_link_libraries(animation ${stdgl_libraries})
FIND_PACKAGE(JPEG REQUIRED)
TARGET_LINK_LIBRARIES(animation ${JPEG_LIBRARIES})
TARGET_LINK_LIBRARIES(animation pmdreader)
