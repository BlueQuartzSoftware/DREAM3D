set(${PLUGIN_NAME}_HSMOOTH_HDRS
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/HierarchicalSmooth/Base.h
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/HierarchicalSmooth/HierarchicalSmooth.h
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/HierarchicalSmooth/Triangulation.h
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/HierarchicalSmooth/Types.h
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/HierarchicalSmooth/VolumeSolver.h
)

set(${PLUGIN_NAME}_HSMOOTH_SRCS
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/HierarchicalSmooth/Base.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/HierarchicalSmooth/HierarchicalSmooth.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/HierarchicalSmooth/Triangulation.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/HierarchicalSmooth/VolumeSolver.cpp
)

cmp_IDE_SOURCE_PROPERTIES("${PLUGIN_NAME}Filters/HierarchicalSmooth" "${${PLUGIN_NAME}_HSMOOTH_HDRS}" "${${PLUGIN_NAME}_HSMOOTH_SRCS}" "0")
