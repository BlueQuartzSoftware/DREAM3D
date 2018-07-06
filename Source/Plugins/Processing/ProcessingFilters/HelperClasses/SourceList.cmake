
set(${PLUGIN_NAME}_HelperClasses_HDRS "")
set(${PLUGIN_NAME}_HelperClasses_SRCS "")

set(${PLUGIN_NAME}_HelperClasses_HDRS ${${PLUGIN_NAME}_HelperClasses_HDRS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/HelperClasses/ComputeGradient.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/HelperClasses/DetectEllipsoidsImpl.h
)

set(${PLUGIN_NAME}_HelperClasses_SRCS ${${PLUGIN_NAME}_HelperClasses_SRCS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/HelperClasses/ComputeGradient.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/HelperClasses/DetectEllipsoidsImpl.cpp
)


# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_SOURCE_PROPERTIES( "${PLUGIN_NAME}/HelperClasses" "${${PLUGIN_NAME}_HelperClasses_HDRS}" "${${PLUGIN_NAME}_HelperClasses_SRCS}" "0")




