include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
        OSInterface
        GIT_REPOSITORY  git@github.com:purifymywater/OSInterface.git
        GIT_TAG         v0.0.1
)
FetchContent_MakeAvailable(OSInterface)

add_library(CANInterface STATIC)

file(GLOB CANInterface_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

target_sources(CANInterface PRIVATE ${CANInterface_SOURCES})
target_include_directories(CANInterface PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(CANInterface PUBLIC OSInterface)
