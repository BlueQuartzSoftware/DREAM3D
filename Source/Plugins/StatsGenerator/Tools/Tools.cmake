set(PLUGIN_NAME "StatsGenerator")
set(${PLUGIN_NAME}Tools_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Tools)
set(${PLUGIN_NAME}Tools_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Tools)

set(GenerateSGPipelineFile_SRCS
  ${${PLUGIN_NAME}Tools_SOURCE_DIR}/GenerateSGPipelineFile.cpp
  )

set(GenerateSGPipelineFile_HDRS
  ${${PLUGIN_NAME}Tools_SOURCE_DIR}/GenerateSGPipelineFile.h
  )


add_executable(GenerateSGPipelineFile ${GenerateSGPipelineFile_SRCS} ${GenerateSGPipelineFile_HDRS})
target_link_libraries(GenerateSGPipelineFile SIMPLib)
