set(PLUGIN_NAME "StatsGenerator")
set(${PLUGIN_NAME}Examples_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Examples)
set(${PLUGIN_NAME}Examples_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Examples)


configure_file(${${PLUGIN_NAME}Examples_SOURCE_DIR}/StatsGeneratorExamples.h.in
               ${${PLUGIN_NAME}Examples_BINARY_DIR}/StatsGeneratorExample.h
    )

include_directories(${${PLUGIN_NAME}Examples_BINARY_DIR})
include_directories(${${PLUGIN_NAME}_BINARY_DIR}/Examples)


set(${PLUGIN_NAME}Examples_SRCS
    ${${PLUGIN_NAME}Examples_SOURCE_DIR}/ModifySGPipeline.cpp
    ${${PLUGIN_NAME}Examples_BINARY_DIR}/StatsGeneratorExample.h
    )

add_executable(ModifySGPipeline ${${PLUGIN_NAME}Examples_SRCS})
target_link_libraries(ModifySGPipeline  Qt5::Core)
