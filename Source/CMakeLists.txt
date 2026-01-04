include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

add_library(CANInterface STATIC)

file(GLOB CANInterface_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

target_sources(CANInterface PRIVATE ${CANInterface_SOURCES})
target_include_directories(CANInterface PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
