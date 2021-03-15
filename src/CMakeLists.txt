file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

add_executable(entler ${SOURCE_FILES} ${HEADER_FILES})
include_directories(${CMAKE_SOURCE_DIR}/libs)
include_directories(${CMAKE_SOURCE_DIR}/src)
target_link_libraries(entler behavior_tree)
